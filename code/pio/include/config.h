#include <pgmspace.h>

#define THINGNAME "ESP32-thing"

const char WIFI_SSID[] = "OayOaRyLala";
const char WIFI_PASSWORD[] = "maman'lah5";
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
MIIDWjCCAkKgAwIBAgIVAIE8Vzpzp6f+XzpZf0i0+WKps7jrMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNDA4MDIxOTUy
MjJaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDh4xJ4dsq0URz+F5ia
hlUortJZr/F4+R6fKonhX9YCd209TavJDZeEoyJXgiyjn5bvpoiDnCoV0cr9T4Y/
tMoIJ5y+r2wURUKCYDHNGhMs5ykBzhsyMsVv2uKl4aW6cFG8DwDDyGAqclP+LKWN
dhfpVFPa8MFRQvfmcFVzGgReU4e87sTLLk/gV77ZCodVrMhcXVimLv/f5C1V5Gay
t7ksaygyPYs8jlQ9svpsufygPvfuFZRzk3c3dBPiZIF+ixhKrKk8GvuwxYG6Fpwp
F6wZKuVsdLV01OSW+kjLqecrvWa5EipeLYCM9j02XqppQEJENVtK7XZr/QD8V4u2
FzwjAgMBAAGjYDBeMB8GA1UdIwQYMBaAFFhShkG8ZXhGMZ9oRWFOAGxM4ZNLMB0G
A1UdDgQWBBSKXOKh1iSKuOUYVaaHI1uAyBBj5DAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAA1p3xWSNluQFcgjIr2jj2WeA
4uJUt5PkDsnliEOKhPDBvaL9kqqfU1/YKuTx25stfZ1Fu2i+mzP82aZxP0DNoHIO
mU2LPgW0SbjDjwVN8XNERPSByVth57aQxqLlZs56JzoqMunJ/1jI/X2ukDAMM+Sj
NhDCPrlpZ7jsUg6u6MA+g/4QH/GLcvSuFCGXO/sU+AaYW6QeHtdxwlwiM2u/CWr8
wTbo9mQxdX+bE8iRv9TjofmJ2FWaFsPfLiBLBAw89fhjqPpjzkqUXY0XJVkSjozQ
i6/iW516K7E2X2p2fQoZcn3NKOI6PFK/EG5OG9yRYs5V43bJPVeyzK+HYV/2FQ==
-----END CERTIFICATE-----
)KEY";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA4eMSeHbKtFEc/heYmoZVKK7SWa/xePkenyqJ4V/WAndtPU2r
yQ2XhKMiV4Iso5+W76aIg5wqFdHK/U+GP7TKCCecvq9sFEVCgmAxzRoTLOcpAc4b
MjLFb9ripeGlunBRvA8Aw8hgKnJT/iyljXYX6VRT2vDBUUL35nBVcxoEXlOHvO7E
yy5P4Fe+2QqHVazIXF1Ypi7/3+QtVeRmsre5LGsoMj2LPI5UPbL6bLn8oD737hWU
c5N3N3QT4mSBfosYSqypPBr7sMWBuhacKResGSrlbHS1dNTklvpIy6nnK71muRIq
Xi2AjPY9Nl6qaUBCRDVbSu12a/0A/FeLthc8IwIDAQABAoIBAQCwu8Ub03XNlxkk
BLo1TJ0cc4vI+Evp5p/60IGIgD8lNuOVb0vUxnmiITEhXowmiRrC7n0o2+phn/8+
KhZTidP4hWWyG/Zn9biP4UuVuaz/GO2Qp/3HTMoFNjDWsLHGKVMJ3IMwq3nyO/lT
iQMOpzMWMBBb4LqgJdFnzsRMhjKcyS0Gh+MMncTtj+S/rhI0TYLDMiQfAVHV9kNE
AwUGfIDPRNV4SatrNQvIsWs0DEpvwV9fcLF6QN4ebuAbGUVKddh/62Vwr8OElkcg
YY+VmNOdDF/U+oyA505wxYA/U0Z67OaCgrAiYtuFj62v9NKmfoJm1NOPBgF7Jif+
2gH3BXXBAoGBAP/9G4QYIpM32um1qfPUEIi13zw9ObZjSdO1Zt0Y15ncsHeDATbO
UIiCiFRBTrR3zp+EbiIvj5L87gV6OYGOzzKtUZx0JioNUC2L2xV8H5noTGx2ONPh
OZxRhj0PTM5Xg8HEROoeOMg2PRUFcbBFPel6fWBEDncnZugR5Jv5+TkLAoGBAOHl
n+GO4B0toDp9aNWi4ePSuPJyUSqdu7blqAJf0GOhlF0F6UtmT5MRCHr3ZhjeMKTv
YBFVt5pXfuTrKqiHDb2a8JTHXwUuJLra2lSdOvc4t6pr8oCu4xQ6aPULm8FEmflk
1FjjTGucTdHw42DaA2F9BDILwYnRBQl0J5BzfOhJAoGBAJuAoq1sh9xMStQ7waDX
iq+xY4lRW/Safed4Q6M9iwLxV9VRpHQpgixatvwPrALe4IpwRQLJR3sv4Fs9cx9r
pBwRwwi6WdCjw7fXRF/ZxcyVm48rhbvYDkoSD+yZlbum1+fmFULu1ulwYUyawQQm
G8Tv3qeS0AK70bxZ2jiR8j4fAoGAG7ylL/3qGMsUZHonmCvCzsV9/g75VmE9vkxe
n8QCA2nAnNdv+NkcJjOuNeMQvzbxBB98TUSYIbRimVsixsSmdESFC8+bJDixO9xJ
eKBBsD9fleZhRVmz+jTd1FDTlByHR28Q/8URChmMGJ4aXTcHuT2veTGmVvS6SJnh
jcQQbzkCgYAb7sK1pFN3XLNe2S7BRX4VPzQqHaaUwnLamAWuig9leEdX9aEG0U1l
Nm5AiPUy7f+GCSfsjF3AAcy09LwLAg9DTaCDqDbE2xPLLfaPUVhOg0DVWecEHY0a
00peTq0UfqlXGZixn2IOaXGWlk22CNEbSgM+7Yi/+13dmgRx+ifuxg==
-----END RSA PRIVATE KEY-----
)KEY";