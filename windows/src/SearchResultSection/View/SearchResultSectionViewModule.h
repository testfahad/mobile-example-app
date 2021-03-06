// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#pragma once

#include "ISearchResultSectionViewModule.h"

#include "BidirectionalBus.h"
#include "Menu.h"
#include "SearchResultSection.h"
#include "Types.h"
#include "IMenuReactionModel.h"

namespace ExampleApp
{
    namespace SearchResultSection
    {
        namespace View
        {
            class SearchResultSectionViewModule: public ISearchResultSectionViewModule, private Eegeo::NonCopyable
            {
            private:
                SearchResultSectionController* m_pController;

            public:
                SearchResultSectionViewModule(Menu::View::IMenuViewModel& searchMenuViewModel,
                                              Menu::View::IMenuOptionsModel& menuOptionsModel,
                                              ISearchResultSectionOrder& searchResultSectionOrder,
                                              ExampleAppMessaging::TMessageBus& messageBus,
                                              const Menu::View::IMenuReactionModel& menuReaction);

                ~SearchResultSectionViewModule();
            };
        }
    }
}
