#include "pch.h"

#if 0

#include "wsdd.nsmap"
#include "gsoap/plugin/wsddapi.h"

static struct soap *soap_listen;
static const char* multicast_url = "soap.udp://239.255.255.250:3702";

static std::string endpoint;

void printMatch(const wsdd__ProbeMatchType& match)
{
    std::cout << "===================================================================" << std::endl;

    if (match.wsa5__EndpointReference.Address) {
        std::cout << "Endpoint:\t"<< match.wsa5__EndpointReference.Address << std::endl;
        // endpoint = match.wsa5__EndpointReference.Address;
    }

    if (match.Types) {
        std::cout << "Types:\t\t"<< match.Types<< std::endl;
    }

    if (match.Scopes) {
        if (match.Scopes->__item ) {
            std::cout << "Scopes:\t\t"<< match.Scopes->__item << std::endl;
        }

        if (match.Scopes->MatchBy) {
            std::cout << "MatchBy:\t"<< match.Scopes->MatchBy << std::endl;
        }
    }

    if (match.XAddrs) {
        std::cout << "XAddrs:\t\t"<< match.XAddrs << std::endl;
        endpoint = match.XAddrs;
    }

    std::cout << "MetadataVersion:\t\t" << match.MetadataVersion << std::endl;
    std::cout << "-------------------------------------------------------------------" << std::endl;
}

void printMatch(const wsdd__ResolveMatchType& match)
{
    std::cout << "===================================================================" << std::endl;

    if (match.wsa5__EndpointReference.Address) {
        std::cout << "Endpoint:\t"<< match.wsa5__EndpointReference.Address << std::endl;
    }

    if (match.Types) {
        std::cout << "Types:\t\t"<< match.Types<< std::endl;
    }

    if (match.Scopes) {
        if (match.Scopes->__item ) {
            std::cout << "Scopes:\t\t"<< match.Scopes->__item << std::endl;
        }

        if (match.Scopes->MatchBy) {
            std::cout << "MatchBy:\t"<< match.Scopes->MatchBy << std::endl;
        }
    }

    if (match.XAddrs) {
        std::cout << "XAddrs:\t\t"<< match.XAddrs << std::endl;
    }

    std::cout << "MetadataVersion:\t\t" << match.MetadataVersion << std::endl;
    std::cout << "-------------------------------------------------------------------" << std::endl;
}

void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *matches)
{
    printf("wsdd_event_ProbeMatches tid:%s RelatesTo:%s nbMatch:%d\n", MessageID, RelatesTo, matches->__sizeProbeMatch);
    for (int i=0; i < matches->__sizeProbeMatch; ++i) {
        wsdd__ProbeMatchType& elt = matches->ProbeMatch[i];
        printMatch(elt);
    }
}


void wsdd_event_ResolveMatches(
    struct soap *soap,
    unsigned int InstanceId,
    const char *SequenceId,
    unsigned int MessageNumber,
    const char *MessageID,
    const char *RelatesTo,
    struct wsdd__ResolveMatchType *match)
{
    printf("wsdd_event_ResolveMatches tid:%s RelatesTo:%s\n", MessageID, RelatesTo);
    printMatch(*match);
}

void wsdd_event_Hello(
    struct soap *soap,
    unsigned int InstanceId,
    const char *SequenceId,
    unsigned int MessageNumber,
    const char *MessageID,
    const char *RelatesTo,
    const char *EndpointReference,
    const char *Types,
    const char *Scopes,
    const char *MatchBy,
    const char *XAddrs,
    unsigned int MetadataVersion)
{
}

void wsdd_event_Bye(
    struct soap *soap,
    unsigned int InstanceId,
    const char *SequenceId,
    unsigned int MessageNumber,
    const char *MessageID,
    const char *RelatesTo,
    const char *EndpointReference,
    const char *Types,
    const char *Scopes,
    const char *MatchBy,
    const char *XAddrs,
    unsigned int *MetadataVersion)
{
}

soap_wsdd_mode wsdd_event_Resolve(
    struct soap *soap,
    const char *MessageID,
    const char *ReplyTo,
    const char *EndpointReference,
    struct wsdd__ResolveMatchType *match)
{
    return SOAP_WSDD_ADHOC;
}

soap_wsdd_mode wsdd_event_Probe(
    struct soap *soap,
    const char *MessageID,
    const char *ReplyTo,
    const char *Types,
    const char *Scopes,
    const char *MatchBy,
    struct wsdd__ProbeMatchesType *matches)
{
    return SOAP_WSDD_ADHOC;
}

void discovery_init()
{
    soap_listen = soap_new1(SOAP_IO_UDP);

    soap_listen->connect_flags |= SO_BROADCAST;

    if (!soap_valid_socket(soap_bind(soap_listen, NULL, 0, 1000))) {
        soap_print_fault(soap_listen, stderr);
    }
}

void probe_do(int milliseconds)
{
    const char *type = "";
    const char *scope = "onvif://www.onvif.org/";

    soap_status probeResult = soap_wsdd_Probe(
        soap_listen,
        SOAP_WSDD_ADHOC,
        SOAP_WSDD_TO_TS,
        multicast_url,
        soap_wsa_rand_uuid(soap_listen),
        NULL,
        type,
        scope,
        "");
    if (probeResult != SOAP_OK) {
        soap_print_fault(soap_listen, stderr);
    }

    soap_wsdd_listen(soap_listen, -milliseconds * 1000);
}

void resolve_do(int milliseconds)
{
    soap_status probeResult = soap_wsdd_Resolve(
        soap_listen,
        SOAP_WSDD_ADHOC,
        SOAP_WSDD_TO_TS,
        multicast_url,
        soap_wsa_rand_uuid(soap_listen),
        NULL,
        endpoint.c_str()
        );
    if (probeResult != SOAP_OK) {
        soap_print_fault(soap_listen, stderr);
    }

    soap_wsdd_listen(soap_listen, -milliseconds * 1000);
}

void discovery_destruct()
{
    soap_destroy(soap_listen);
    soap_end(soap_listen);
    soap_free(soap_listen);
}

int main(int argc, char *argv[])
{
    discovery_init();
    probe_do(3000);
    resolve_do(3000);
    discovery_destruct();

    return 0;
}

#else

#include "onvif/DeviceBinding.nsmap"
#include "onvif/soapDeviceBindingProxy.h"
#include "onvif/soapMediaBindingProxy.h"
#include "onvif/soapPullPointSubscriptionBindingProxy.h"
#include "onvif/soapRuleEngineBindingProxy.h"


namespace {
    //char const *const IpCamUrl = "http://door.cam:8899/";
    //char const *const IpCamUrl = "http://dev.cam:8899/";
    char const *const IpCamUrl = "http://192.168.88.21:8899/onvif/device_service";
}

int main(int argc, char *argv[])
{
    soap_status status;

    soap* soap = soap_new();

    DeviceBindingProxy deviceProxy(soap, IpCamUrl);

    _tds__GetDeviceInformation getDeviceInformation;
    _tds__GetDeviceInformationResponse getDeviceInformationResponse;
    status = deviceProxy.GetDeviceInformation(&getDeviceInformation, getDeviceInformationResponse);

    std::cout << "Manufacturer:    " << getDeviceInformationResponse.Manufacturer << std::endl;
    std::cout << "Model:           " << getDeviceInformationResponse.Model << std::endl;
    std::cout << "FirmwareVersion: " << getDeviceInformationResponse.FirmwareVersion << std::endl;
    std::cout << "SerialNumber:    " << getDeviceInformationResponse.SerialNumber << std::endl;
    std::cout << "HardwareId:      " << getDeviceInformationResponse.HardwareId << std::endl;

    _tds__GetCapabilities getCapabilities;
    _tds__GetCapabilitiesResponse getCapabilitiesResponse;
    status = deviceProxy.GetCapabilities(&getCapabilities, getCapabilitiesResponse);

    if(!getCapabilitiesResponse.Capabilities || !getCapabilitiesResponse.Capabilities->Media)
    {
      std::cerr << "Missing device capabilities info" << std::endl;
      exit(EXIT_FAILURE);
    }

    std::cout << "XAddr:        " << getCapabilitiesResponse.Capabilities->Media->XAddr << std::endl;
    if(getCapabilitiesResponse.Capabilities->Media->StreamingCapabilities) {
        if(getCapabilitiesResponse.Capabilities->Media->StreamingCapabilities->RTPMulticast)
            std::cout << "RTPMulticast: " << (*getCapabilitiesResponse.Capabilities->Media->StreamingCapabilities->RTPMulticast ? "yes" : "no") << std::endl;
        if(getCapabilitiesResponse.Capabilities->Media->StreamingCapabilities->RTP_USCORETCP)
            std::cout << "RTP_TCP:      " << (*getCapabilitiesResponse.Capabilities->Media->StreamingCapabilities->RTP_USCORETCP ? "yes" : "no") << std::endl;
        if(getCapabilitiesResponse.Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP)
            std::cout << "RTP_RTSP_TCP: " << (*getCapabilitiesResponse.Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP ? "yes" : "no") << std::endl;
    }

    char const *const media_endpoint = getCapabilitiesResponse.Capabilities->Media->XAddr.c_str();

    std::cout << "Media Url: " << media_endpoint << std::endl;

    MediaBindingProxy mediaProxy(soap, media_endpoint);

    _trt__GetProfiles getProfiles;
    _trt__GetProfilesResponse getProfilesResponse;
    status = mediaProxy.GetProfiles(&getProfiles, getProfilesResponse);

    for (int i = 0; i < getProfilesResponse.Profiles.size(); ++i) {
      _trt__GetSnapshotUri getSnapshotUri;
      _trt__GetSnapshotUriResponse getSnapshotUriResponse;
      getSnapshotUri.ProfileToken = getProfilesResponse.Profiles[i]->token;
      status = mediaProxy.GetSnapshotUri(&getSnapshotUri, getSnapshotUriResponse);

      std::cout << "Profile name: " << getProfilesResponse.Profiles[i]->Name << std::endl;
      if(getSnapshotUriResponse.MediaUri) {
        std::cout << "Snapshot media Url: " << getSnapshotUriResponse.MediaUri->Uri << std::endl;
      }
    }

    for (int i = 0; i < getProfilesResponse.Profiles.size(); ++i) {
      _trt__GetStreamUri getStreamUri;
      _trt__GetStreamUriResponse getStreamUriResponse;
      getStreamUri.ProfileToken = getProfilesResponse.Profiles[i]->token;

      tt__StreamSetup streamSetup;
      streamSetup.Stream = tt__StreamType::RTP_Unicast;

      getStreamUri.StreamSetup = &streamSetup;
      status = mediaProxy.GetStreamUri(&getStreamUri, getStreamUriResponse);

      std::cout << "Profile name: " << getProfilesResponse.Profiles[i]->Name << std::endl;
      if(getStreamUriResponse.MediaUri) {
        std::cout << "Stream media Url: " << getStreamUriResponse.MediaUri->Uri << std::endl;
      }
    }

    PullPointSubscriptionBindingProxy pullProxy(soap, media_endpoint);

    _tev__GetServiceCapabilities getServiceCapabilities;
    _tev__GetServiceCapabilitiesResponse getServiceCapabilitiesResponse;
    status = pullProxy.GetServiceCapabilities(&getServiceCapabilities, getServiceCapabilitiesResponse);

    _tev__GetEventProperties getEventProperties;
    _tev__GetEventPropertiesResponse getEventPropertiesResponse;
    status = pullProxy.GetEventProperties(&getEventProperties, getEventPropertiesResponse);
    wstop__TopicSetType* topicSet = getEventPropertiesResponse.wstop__TopicSet;

    /* topicSet->__any:
    <tns1:RuleEngine wstop:topic="true">
        <CellMotionDetector wstop:topic="true">
            <Motion wstop:topic="true">
                <tt:MessageDescription IsProperty="true">
                    <tt:Source>
                        <tt:SimpleItemDescription Name="VideoSourceConfigurationToken" Type="tt:ReferenceToken"/>
                        <tt:SimpleItemDescription Name="VideoAnalyticsConfigurationToken" Type="tt:ReferenceToken"/>
                        <tt:SimpleItemDescription Name="Rule" Type="xsd:string"/>
                    </tt:Source>
                    <tt:Data>
                        <tt:SimpleItemDescription Name="IsMotion" Type="xsd:boolean"/>
                    </tt:Data>
                </tt:MessageDescription>
            </Motion>
        </CellMotionDetector>
    </tns1:RuleEngine>
    <tns1:VideoSource wstop:topic="true">
        <SignalLoss wstop:topic="true">
            <tt:MessageDescription IsProperty="true">
                <tt:Source>
                    <tt:SimpleItemDescription Name="Source" Type="tt:ReferenceToken"/>
                </tt:Source>
                <tt:Data>
                    <tt:SimpleItemDescription Name="State" Type="xsd:boolean"/>
                </tt:Data>
            </tt:MessageDescription>
        </SignalLoss>
        <MotionAlarm wstop:topic="true">
            <tt:MessageDescription IsProperty="true">
                <tt:Source>
                    <tt:SimpleItemDescription Name="Source" Type="tt:ReferenceToken"/>
                </tt:Source>
                <tt:Data>
                    <tt:SimpleItemDescription Name="State" Type="xsd:boolean"/>
                </tt:Data>
            </tt:MessageDescription>
        </MotionAlarm>
        <tnsn:SignalStandardMismatch wstop:topic="true">
            <tt:MessageDescription>
                <tt:Source>
                    <tt:SimpleItemDescription Name="VideoSourceToken" Type="tt:ReferenceToken"/>
                </tt:Source>
                <tt:Data>
                    <tt:SimpleItemDescription Name="State" Type="xsd:boolean"/>
                </tt:Data>
            </tt:MessageDescription>
        </tnsn:SignalStandardMismatch>
    </tns1:VideoSource>*/

    RuleEngineBindingProxy ruleEngineBindingProxy(soap, media_endpoint);

    _tan__GetSupportedAnalyticsModules getSupportedAnalyticsModules;
    _tan__GetSupportedAnalyticsModulesResponse getSupportedAnalyticsModulesResponse;
    status = ruleEngineBindingProxy.GetSupportedAnalyticsModules(&getSupportedAnalyticsModules, getSupportedAnalyticsModulesResponse);


    // supported sensitivity: 16, 33, 50, 66, 83, 100
    const unsigned sensitivity = 50;
    {
        _tan__GetAnalyticsModules tan__GetAnalyticsModules;
        _tan__GetAnalyticsModulesResponse tan__GetAnalyticsModulesResponse;
        status = ruleEngineBindingProxy.GetAnalyticsModules(&tan__GetAnalyticsModules, tan__GetAnalyticsModulesResponse);

        tt__Config* config = tan__GetAnalyticsModulesResponse.AnalyticsModule[0];
        auto oldSensitifity = tan__GetAnalyticsModulesResponse.AnalyticsModule[0]->Parameters->SimpleItem[0].Value;

        auto sensitifityToSet = std::to_string(sensitivity);
        config->Parameters->SimpleItem[0].Value = sensitifityToSet;

        _tan__ModifyAnalyticsModules tan__ModifyAnalyticsModules;
        tan__ModifyAnalyticsModules.AnalyticsModule.push_back(config);
        _tan__ModifyAnalyticsModulesResponse tan__ModifyAnalyticsModulesResponse;
        status = ruleEngineBindingProxy.ModifyAnalyticsModules(&tan__ModifyAnalyticsModules, tan__ModifyAnalyticsModulesResponse);

        status = ruleEngineBindingProxy.GetAnalyticsModules(&tan__GetAnalyticsModules, tan__GetAnalyticsModulesResponse);
        auto retreivedEnsitifity = tan__GetAnalyticsModulesResponse.AnalyticsModule[0]->Parameters->SimpleItem[0].Value;

        std::cout << "set sensitivity:" << sensitifityToSet << " retreived sensitifity:" << retreivedEnsitifity << std::endl;
    }


    _tan__GetServiceCapabilities tanGetServiceCapabilities;
    _tan__GetServiceCapabilitiesResponse tanGetServiceCapabilitiesResponse;
    status = ruleEngineBindingProxy.GetServiceCapabilities(&tanGetServiceCapabilities, tanGetServiceCapabilitiesResponse);

    _tan__GetSupportedRules getSupportedRules;
    _tan__GetSupportedRulesResponse getSupportedRulesResponse;
    status = ruleEngineBindingProxy.GetSupportedRules(&getSupportedRules, getSupportedRulesResponse);

    _tan__GetRules getRules;
    _tan__GetRulesResponse getRulesResponse;
    status = ruleEngineBindingProxy.GetRules(&getRules, getRulesResponse);



    _tev__CreatePullPointSubscription createPullPointSubscription;
    _tev__CreatePullPointSubscriptionResponse createPullPointSubscriptionResponse;
    status = pullProxy.CreatePullPointSubscription(&createPullPointSubscription, createPullPointSubscriptionResponse);

    while(true) {
        _tev__PullMessages pullMessages;
        _tev__PullMessagesResponse pullMessagesResponse;
        status = pullProxy.PullMessages(&pullMessages, pullMessagesResponse);

        if(status == SOAP_OK) {
            for(const wsnt__NotificationMessageHolderType* messageHolder: pullMessagesResponse.wsnt__NotificationMessage) {
                const soap_dom_element& message = messageHolder->Message.__any;
                /* message
                <tt:Message UtcTime="2022-07-02T21:39:37Z" PropertyOperation="Changed">
                    <tt:Source>
                        <tt:SimpleItem Name="VideoSourceConfigurationToken" Value="000" />
                        <tt:SimpleItem Name="VideoAnalyticsConfigurationToken" Value="000" />
                        <tt:SimpleItem Name="Rule" Value="MyMotionDetectorRule" />
                    </tt:Source>
                    <tt:Data>
                        <tt:SimpleItem Name="IsMotion" Value="false" />
                    </tt:Data>
                </tt:Message>*/
                soap_dom_element* data = message.elt_get("tt:Data");
                if(!data) continue;

                soap_dom_element* simpleItem = data->elt_get("tt:SimpleItem");
                for(;simpleItem; simpleItem = simpleItem->get_next()) {
                    const soap_dom_attribute* name = simpleItem->att_get("Name");
                    if(!name || !name->get_text()) continue;
                    if(name->get_text() != std::string("IsMotion")) continue;

                    const soap_dom_attribute* value = simpleItem->att_get("Value");
                    if(!value || !value->get_text()) continue;

                    std::cout << "IsMotion: " << value->get_text() << std::endl;
                }
            }
        }

        sleep(1);
    }

    soap_destroy(soap);
    soap_end(soap);

    soap_free(soap);
    return 0;
}

#endif
