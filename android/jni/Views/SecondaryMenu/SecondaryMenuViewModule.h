// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#pragma once

#include <string>
#include "AndroidNativeState.h"
#include "Types.h"
#include "Menu.h"
#include "PrimaryMenu.h"
#include "MenuViewIncludes.h"
#include "IMenuViewModule.h"
#include "Search.h"

namespace ExampleApp
{
    namespace SecondaryMenu
    {
        class SecondaryMenuViewModule: public Menu::IMenuViewModule, private Eegeo::NonCopyable
        {
        private:
        	Menu::IMenuViewController* m_pMenuViewController;
            
        public:
        	SecondaryMenuViewModule(
        		const std::string& viewName,
            	AndroidNativeState& nativeState,
        		Menu::IMenuModel& menuModelModel,
        		Menu::IMenuViewModel& menuViewModel,
    			Search::ISearchService& searchService,
    			Search::ISearchQueryPerformer& searchQueryPerformer
            );
            
            ~SecondaryMenuViewModule();
            
            Menu::IMenuViewController& GetMenuViewController() const;
        };
    }
}