// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "GeoNamesSearchQueryFactory.h"
#include "GeoNamesSearchQuery.h"

namespace ExampleApp
{
    namespace Search
    {
        namespace GeoNames
        {
            namespace SdkModel
            {
                GeoNamesSearchQueryFactory::GeoNamesSearchQueryFactory(Eegeo::Web::IWebLoadRequestFactory& webRequestFactory,
                                                                       Eegeo::Helpers::UrlHelpers::IUrlEncoder& urlEncoder)
                : m_webRequestFactory(webRequestFactory)
                , m_urlEncoder(urlEncoder)
                {
                    
                }
                
                GeoNamesSearchQueryFactory::~GeoNamesSearchQueryFactory()
                {
                    
                }
                
                IGeoNamesSearchQuery* GeoNamesSearchQueryFactory::CreateGeoNamesSearchForQuery(const Search::SdkModel::SearchQuery& query,
                                                                                               Eegeo::Helpers::ICallback0& completionCallback)
                {
                    return Eegeo_NEW(GeoNamesSearchQuery)(m_webRequestFactory,
                                                          m_urlEncoder,
                                                          query,
                                                          completionCallback);
                }
            }
        }
    }
}
