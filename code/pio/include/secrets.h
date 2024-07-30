#include <pgmspace.h>

#define SECRET
#define THINGNAME "ESP32-thing"

const char WIFI_SSID[] = "";
const char WIFI_PASSWORD[] = "";
const char AWS_IOT_ENDPOINT[] = "a395l7lep46wzc-ats.iot.us-east-1.amazonaws.com";

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUbnfHGV9NLoyjR+PJn4qF+eizR/0wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MDcyNzA1NDAx
OVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALfWAyBzIlweWsujuSn0
57Z7PCuIo/nkVMTXlQAEYGFMzeWVWn15UEJUjJoYcG80HY+dQEXUzqFCQBqS9Dxw
lnta22YwNXst47xUQ45uM7Xk/XzOdTm2bXSwERvANGAvwnbKK+rcjbASapToFGJg
6TORaVcipPRbEW/n8SI4AurhJdufh7l90yzZeEpmwXZJg84pnRtekU51lBVTg0nC
19hZ9fSD8fgv1dz8sEjN7fFcLcZbOCzlsEP9GJqzxg9iEkTGuEkrrryTy2/DPtGF
n1735+MUm0maCWVlLVJgCkUjojbczm4/HPy74AIEthCfKe8Zkrj7IiU7THjiD+Rh
vF8CAwEAAaNgMF4wHwYDVR0jBBgwFoAU+bHTh4qwlHj4uPiCSJgu97DAWzEwHQYD
VR0OBBYEFGW17iSxnfluAvWce4NQpY7tfMtwMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQB87sLD5bvgGZb60CVfJuStxVn8
X6K3I7AAEjXOjLzlJ1LJ1AMaiAlBqGAKxTo+fps56xqRTfVA3N3eHJMaT9F2KZPi
eKFBzu4SOUxhBgEjrtxGM4mS7ZLh66IQD5iE9phTAAxMSOxKTkVPfRwM5d4otnxL
GJYOe/gmwJbgZ9Yt2/bdOelBSam/pyF1FtW5RzKxzc7KekG1WuFaT5NpI+c6cxRN
a1ie8H1VtUSWgdVPpy1B4ClJFJh3TNVmsBnEcRao3O9zm//TMLEwZuR90ukjtcK7
wXMHNtlASkqp20IqHMgB9HamtkW22z2lfRF+3Scf15A51F5O0S/wyMoIJMEt
-----END CERTIFICATE-----

)KEY";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAt9YDIHMiXB5ay6O5KfTntns8K4ij+eRUxNeVAARgYUzN5ZVa
fXlQQlSMmhhwbzQdj51ARdTOoUJAGpL0PHCWe1rbZjA1ey3jvFRDjm4zteT9fM51
ObZtdLARG8A0YC/Cdsor6tyNsBJqlOgUYmDpM5FpVyKk9FsRb+fxIjgC6uEl25+H
uX3TLNl4SmbBdkmDzimdG16RTnWUFVODScLX2Fn19IPx+C/V3PywSM3t8Vwtxls4
LOWwQ/0YmrPGD2ISRMa4SSuuvJPLb8M+0YWfXvfn4xSbSZoJZWUtUmAKRSOiNtzO
bj8c/LvgAgS2EJ8p7xmSuPsiJTtMeOIP5GG8XwIDAQABAoIBADb1s3+bDDWwGYLy
EqR5hGCo3SdCtYs6+SDUNsaHiGzuckvl5+TFRPpfnp1fGxYT5ZaO860s3w2U6edH
8d6z5jlAw4D8frCx2njuVDgbuDf0GtiaPK+hW33cOXEreD0daizrak/P2kg+aUDV
VWrIAFAbc/vJ7rjjFQZ4uD17s1uWrFNEjUzPJvYng9EycxZ60Emq+nYr+lyO1Yo9
lD1IdxQ/Y1k20xLwm2kkgMtGct3tA5s9r0O6UILgQbijhKzeMyixNbFFDn9ZV3Tw
JNMDfKfMEgZlKUAXvQmXaQluWSGxnBF6cx5z9A2SiK7DMsR4OUUw3dXyIHRhQnbW
Zk37NVkCgYEA4agM3ZvjLmuy5ColDpsm9EtVbAJNskHWeSe1UK7Wj2FCtYY6aoWb
krxJsOglgUapBXdHTFg5hXxZ12u+Y6KEXtiXkI5oNFqKI2Zz0+Zd5vPmTfQ83DjB
dUUz5Ssit643H/XRRRsLJiWPCUCnvP/A1vKjNQEY52yVUh13bl3qwR0CgYEA0I5X
36eqWg2DvSFqf+0TZOWagRKARvSQjGrkZPnZvOLTeJs0m1mZ8PkBk6BFxnAJGRIb
yxhmBrqCcDjVTblNmMhb91cdAjKVokHxaWNHZXw9rMSV0gwXFYoB3t7Sxqc/HU5M
2FKZ28Yx5F25aDwd3fMXo1X36dzo8VPJrvcPVqsCgYEA0YnzKHVw59IqM6TO9Fku
OvsZP0qES1rHhYlR3LH9mvZ9s4IJDJE0fXVKGHP5H6o1mVXBZyslfsaRxm/OQ0bO
FCSb2Sfjq3nHfEfOlp375FHnMcvyF6y8N+Ei51K+0kF7/rmMsdcC61FFGRpoqB/1
OaPl2XpDXeJ+Zdnm5R1guCECgYB+JRrRlMBdJ79/Li8UoXkRli8V0vLz+AlWZXEw
Ak/JCAmQ8Y+oR1Hthy+E83RAypM2LtXQyjkr6LPs3PNnx6U/vDCkMnTBV4daVTiH
3hzZLYTAc0eLfPSZepgXDf+/WHm8aBfi4FNfs0mKL4Ztna8TCG5LxTvmoE/laCCd
S6JjcQKBgFanMoDs4DbknGqEswL3yQKUqxqfdxfWXd2LE5b4yNgyvSLzxQ3S1g7v
clTjybSY1UqBejr1/bn2M/aSym9VY/hcSz8vOC8aH1AGiyudQgqyV840SDvy27si
/QFt75cZfxgHThCxG6TZ/g1443kN6Hu0tjIGsaAkxauMPuZH7nLB
-----END RSA PRIVATE KEY-----
)KEY";