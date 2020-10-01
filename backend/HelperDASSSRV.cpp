#include "HelperDASSSRV.h"


void HelperDASSSRV::saveVal(const std::string &valueName, const std::string &value, ObjectData *od){


    std::string error   = "";

    GetGPQConn::get()->setData( HelperDASSSRV::M_GPQ_DAD,
                                HelperDASSSRV::T_VALUES,

                /*section set*/ {HelperDASSSRV::C_VALUES_OBJECT_NAME,
                                 HelperDASSSRV::C_VALUES_VALUE},
                                {od->getShow(),
                                 value},

              /*section where*/ {HelperDASSSRV::C_VALUES_URI,
                                 HelperDASSSRV::C_VALUES_VALUE_NAME,},
                                {od->getId(),
                                 valueName},
                                 error);

    if( ! error.empty()){

         InstanceLogger::instance()->error(error.c_str());
    }


    return;
}

std::string HelperDASSSRV::ValuePtrToStr(const ValuePtr &valuePtr){

    std::string val = "";
    switch (valuePtr->type() ) {
    case Value::BOOL:{

            bool  b = toBool(valuePtr);
            val = (b ? "t": "f");
        }break;

    case Value::INT:{

            int  i = toInt(valuePtr);
            val = i2s(i);
        }break;

    case Value::LLINT:{

            long long int  lli = toLLInt(valuePtr);
            val = ll2s(lli);
        }break;

    case Value::REAL:{

            double  real = toReal(valuePtr);
            val = r2s(real);
        }break;

    case Value::STR:{

            val = toStr(valuePtr);
        }break;

    default:
        break;
    }

    return val;
}
