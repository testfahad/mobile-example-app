// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "TourService.h"

#include <sstream>
#include "IMetricsService.h"
#include "ITourRepository.h"
#include "Logger.h"
#include "ITourStateMachine.h"
#include "IToursCameraTransitionController.h"
#include "ISearchRefreshService.h"
#include "TourOnMapSelectedMessage.h"
#include "WorldPinsVisibilityMessage.h"
#include "WorldPinVisibility.h"

namespace ExampleApp
{
    namespace Tours
    {
        namespace SdkModel
        {
            namespace
            {
                const std::string TourMetricsPrefix = "Tour: ";
                const std::string TourCardEnteredKey = "card entered";
                const std::string TourCardExitedKey = "card exit";
            }
            
            TourService::TourService(ITourRepository& repository,
                                     Camera::IToursCameraTransitionController& cameraTransitionController,
                                     Metrics::IMetricsService& metricsService,
                                     Search::SdkModel::ISearchRefreshService& searchRefreshService,
                                     ExampleAppMessaging::TMessageBus& messageBus,
                                     ExampleAppMessaging::TSdkModelDomainEventBus& sdkDomainEventBus)
            : m_repository(repository)
            , m_cameraTransitionController(cameraTransitionController)
            , m_messageBus(messageBus)
            , m_activeTourModel(TourModel::Empty())
            , m_hasActiveTour(false)
            , m_activeTourState(-1)
            , m_pActiveTourStateMachine(NULL)
            , m_nextTourModel(TourModel::Empty())
            , m_suspendCurrentTour(false)
            , m_sdkDomainEventBus(sdkDomainEventBus)
            , m_metricsService(metricsService)
            , m_searchRefreshService(searchRefreshService)
            {
                
            }
            
            TourService::~TourService()
            {
                while(m_repository.GetItemCount() != 0)
                {
                    const TourModel& tourModel(m_repository.GetItemAtIndex(0));
                    RemoveTour(tourModel);
                }
            }
            
            bool TourService::IsTourActive() const
            {
                return m_hasActiveTour;
            }
            
            void TourService::AddTour(const TourModel& tourModel, States::ITourStateMachine& tourStateMachine)
            {
                m_repository.AddItem(tourModel);
                Eegeo_ASSERT(m_pTourToStateMachineMapping.find(tourModel.Name()) == m_pTourToStateMachineMapping.end());
                m_pTourToStateMachineMapping[tourModel.Name()] = &tourStateMachine;
            }
            
            void TourService::RemoveTour(const TourModel& tourModel)
            {
                std::map<std::string, States::ITourStateMachine*>::iterator pMapIt(m_pTourToStateMachineMapping.find(tourModel.Name()));
                Eegeo_ASSERT(pMapIt != m_pTourToStateMachineMapping.end());
                Eegeo_DELETE pMapIt->second;
                m_pTourToStateMachineMapping.erase(pMapIt);
                m_repository.RemoveItem(tourModel);
            }
            
            void TourService::StartCurrentActiveTour(const TourModel& tourModel, const int atCard)
            {
                Eegeo_ASSERT(!m_hasActiveTour);
                Eegeo_ASSERT(m_pTourToStateMachineMapping.find(tourModel.Name()) != m_pTourToStateMachineMapping.end());

                m_activeTourModel = tourModel;
                Eegeo_ASSERT(atCard >= 0 && atCard < m_activeTourModel.StateCount(),
                             "Invalid 'atCard' parameter. Must be within range. Was: %d, Max: %d", atCard, m_activeTourModel.StateCount());
                
                m_cameraTransitionController.ResetControllerState();
                
                m_pActiveTourStateMachine = m_pTourToStateMachineMapping[tourModel.Name()];
                m_pActiveTourStateMachine->StartTour(atCard);
                
                m_hasActiveTour = true;
                m_activeTourState = atCard;
                
                m_metricsService.BeginTimedEvent(TourMetricsPrefix + m_activeTourModel.Name());

                std::stringstream ss;
                ss << atCard;
                m_metricsService.SetEvent(TourMetricsPrefix + m_activeTourModel.Name(), TourCardEnteredKey, ss.str());
                
                m_searchRefreshService.SetEnabled(false);
                m_pActiveTourStateMachine = m_pTourToStateMachineMapping[tourModel.Name()];
                m_pActiveTourStateMachine->StartTour(atCard);

                bool showBackButton = !m_previousActiveToursStack.empty();
                
                m_messageBus.Publish(TourOnMapSelectedMessage(m_activeTourModel, atCard, showBackButton));
                m_sdkDomainEventBus.Publish(WorldPins::WorldPinsVisibilityMessage(WorldPins::SdkModel::WorldPinVisibility::TourPin));
                if(m_previousActiveToursStack.size() == 0)
                {
                    m_tourStartedCallbacks.ExecuteCallbacks();
                }
            }
            
            void TourService::EnqueueNextTour(const TourModel& tourModel)
            {
                if(tourModel != m_activeTourModel)
                {
                    m_nextTourModel = tourModel;
                    m_suspendCurrentTour = true;
                }
            }
            
            void TourService::EndCurrentActiveTour()
            {
                Eegeo_ASSERT(m_hasActiveTour);
                
                std::stringstream ss;
                ss << m_activeTourState;
                m_metricsService.SetEvent(TourMetricsPrefix + m_activeTourModel.Name(), TourCardExitedKey, ss.str());
                
                m_metricsService.EndTimedEvent(TourMetricsPrefix + m_activeTourModel.Name());
                
                m_activeTourModel = TourModel::Empty();
                m_hasActiveTour = false;
                m_activeTourState = -1;
                
                m_pActiveTourStateMachine->EndTour();
                m_pActiveTourStateMachine = NULL;
                
                if(!m_suspendCurrentTour)
                {
                    // Don't return to app camera if we're going to another tour.
                    if(m_previousActiveToursStack.size() == 0)
                    {
                        m_searchRefreshService.SetEnabled(true);
                        m_sdkDomainEventBus.Publish(WorldPins::WorldPinsVisibilityMessage(WorldPins::SdkModel::WorldPinVisibility::World | WorldPins::SdkModel::WorldPinVisibility::Search | WorldPins::SdkModel::WorldPinVisibility::UserPin));
                        
                        m_tourEndedCallbacks.ExecuteCallbacks();
                
                    }
                    else if(m_previousActiveToursStack.size() > 0)
                    {
                        SuspendedTour previousTour = m_previousActiveToursStack.top();
                        m_previousActiveToursStack.pop();
                        
                        StartCurrentActiveTour(previousTour.tour, previousTour.card);
                    }
                }
            }
            
            void TourService::EndAllTours()
            {
                m_suspendCurrentTour = false;
                while(m_previousActiveToursStack.size() != 0)
                {
                    m_previousActiveToursStack.pop();
                }
                EndCurrentActiveTour();
            }
            
            void TourService::SetActiveTourState(int activeStateIndex)
            {
                Eegeo_ASSERT(m_hasActiveTour);
                Eegeo_ASSERT(activeStateIndex < m_activeTourModel.StateCount());
                
                EXAMPLE_LOG("SDK: Active tour state index: %d, name: %s\n",
                            activeStateIndex,
                            m_activeTourModel.States()[activeStateIndex].Headline().c_str());
                
                if(m_activeTourState != activeStateIndex)
                {
                    std::stringstream ss;
                    ss << m_activeTourState;
                    m_metricsService.SetEvent(TourMetricsPrefix + m_activeTourModel.Name(), TourCardExitedKey, ss.str());
                    
                    m_activeTourState = activeStateIndex;
                    m_pActiveTourStateMachine->ChangeToState(activeStateIndex);
                    
                    ss.clear();
                    ss << m_activeTourState;
                    m_metricsService.SetEvent(TourMetricsPrefix + m_activeTourModel.Name(), TourCardEnteredKey, ss.str());
                }
            }
            
            void TourService::UpdateCurrentTour(float dt)
            {
                if(m_hasActiveTour)
                {
                    Eegeo_ASSERT(m_pActiveTourStateMachine != NULL);
                    m_pActiveTourStateMachine->Update(dt);
                    
                    if(m_suspendCurrentTour)
                    {
                        TourModel activeTour = m_activeTourModel;
                        SuspendedTour suspendedTour = SuspendedTour(activeTour, m_pActiveTourStateMachine->GetCurrentState());
                        
                        EndCurrentActiveTour();
                        
                        m_previousActiveToursStack.push(suspendedTour);
                        
                        StartCurrentActiveTour(m_nextTourModel, 0);
                        
                        m_suspendCurrentTour = false;
                    }
                }
            }
            
            void TourService::RegisterTourStartedCallback(Eegeo::Helpers::ICallback0 &callback)
            {
                m_tourStartedCallbacks.AddCallback(callback);
            }
            
            void TourService::UnregisterTourStartedCallback(Eegeo::Helpers::ICallback0 &callback)
            {
                m_tourStartedCallbacks.RemoveCallback(callback);
            }
            
            void TourService::RegisterTourEndedCallback(Eegeo::Helpers::ICallback0 &callback)
            {
                m_tourEndedCallbacks.AddCallback(callback);
            }
            
            void TourService::UnregisterTourEndedCallback(Eegeo::Helpers::ICallback0 &callback)
            {
                m_tourEndedCallbacks.RemoveCallback(callback);
            }
        }
    }
}
