// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#pragma once

#include "Types.h"
#include "IMenuModel.h"
#include "RepositoryModel.h"

namespace ExampleApp
{
    namespace Menu
    {
        namespace View
        {
            class MenuModel : public IMenuModel, private Eegeo::NonCopyable
            {
                typedef MenuItemModel TModel;

                Repository::RepositoryModel<TModel> m_repository;

            public:
                void AddItem(const TModel& item)
                {
                    m_repository.AddItem(item);
                }

                void RemoveItem(const TModel& item)
                {
                    m_repository.RemoveItem(item);
                }

                size_t GetItemCount() const
                {
                    return m_repository.GetItemCount();
                }

                TModel GetItemAtIndex(size_t index)
                {
                    return m_repository.GetItemAtIndex(index);
                }

                void InsertItemAddedCallback(Eegeo::Helpers::ICallback1<TModel>& callback)
                {
                    m_repository.InsertItemAddedCallback(callback);
                }

                void RemoveItemAddedCallback(Eegeo::Helpers::ICallback1<TModel>& callback)
                {
                    m_repository.RemoveItemAddedCallback(callback);
                }

                void InsertItemRemovedCallback(Eegeo::Helpers::ICallback1<TModel>& callback)
                {
                    m_repository.InsertItemRemovedCallback(callback);
                }

                void RemoveItemRemovedCallback(Eegeo::Helpers::ICallback1<TModel>& callback)
                {
                    m_repository.RemoveItemRemovedCallback(callback);
                }
            };
        }
    }
}
