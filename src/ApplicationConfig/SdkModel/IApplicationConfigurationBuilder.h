// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#pragma once

#include <string>
#include "LatLongAltitude.h"
#include "ApplicationConfiguration.h"

namespace ExampleApp
{
    namespace ApplicationConfig
    {
        namespace SdkModel
        {
            class IApplicationConfigurationBuilder
            {
            public:
                virtual ~IApplicationConfigurationBuilder() { } 
                
                virtual IApplicationConfigurationBuilder& SetApplicationName(const std::string& name) = 0;
                
                virtual IApplicationConfigurationBuilder& SetStartInterestPointLocation(const Eegeo::Space::LatLongAltitude& location) = 0;
                
                virtual IApplicationConfigurationBuilder& SetStartDistanceFromInterestPoint(float distanceMetres) = 0;
                
                virtual IApplicationConfigurationBuilder& SetStartOrientationAboutInterestPoint(float degrees) = 0;
                
                virtual IApplicationConfigurationBuilder& SetTryStartAtGpsLocation(bool tryStartAtGpsLocation) = 0;
                
                virtual IApplicationConfigurationBuilder& SetGoogleAnalyticsReferrerToken(const std::string& googleAnalyticsReferrerToken) = 0;
                
                virtual IApplicationConfigurationBuilder& SetProductVersion(const std::string& productVersion) = 0;
                
                virtual IApplicationConfigurationBuilder& SetBuildNumber(const std::string& buildNumber) = 0;
                
                virtual IApplicationConfigurationBuilder& SetCombinedVersionString(const std::string& combinedVersionString) = 0;

                virtual IApplicationConfigurationBuilder& SetCoverageTreeManifestURL(const std::string& coverageTreeManifestUrl) = 0;
                
                virtual IApplicationConfigurationBuilder& SetThemeManifestURL(const std::string& themesManifestUrl) = 0;
                
                virtual ApplicationConfiguration Build() = 0;
            };
        }
    }
}
