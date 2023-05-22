
#ifndef __CCAP_SENSOR_H_
#define __CCAP_SENSOR_H_

#include "NuMicro.h"

int InitHM1055_VGA_YUV422(CCAP_T* ccap);
#define HM1055SensorPolarity         (CCAP_PAR_VSP_LOW | CCAP_PAR_HSP_LOW  | CCAP_PAR_PCLKP_HIGH)
#define HM1055DataFormatAndOrder (CCAP_PAR_INDATORD_YUYV | CCAP_PAR_INFMT_YUV422 | CCAP_PAR_OUTFMT_YUV422)

#endif
