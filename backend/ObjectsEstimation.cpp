#include "ObjectsEstimation.h"


#define REG_MTD(obj, fun) this->regMtd(#fun,       new CallMethod<obj*,ValuePtr (obj::*)(std::vector<ValuePtr> &)>(this,&obj::fun));

//begin  ObjectsEstimation

    ObjectsEstimation::init_::init_(){

        if( ! ObjectsEstimation::_this){

                ObjectsEstimation::_this = new ObjectsEstimation;
            }
    }

    ObjectsEstimation::init_::~init_(){

        if( ObjectsEstimation::_this ){

               delete ObjectsEstimation::_this;
               ObjectsEstimation::_this = nullptr;
           }
    }

    ObjectsEstimation *ObjectsEstimation::_this = nullptr;

    ObjectsEstimation::ObjectsEstimation():_oe_timer(nullptr){

        _oe_timer = new Timer<ObjectsEstimation*, void(ObjectsEstimation::*)()>(this,&ObjectsEstimation::execEstimation);
    }


    ObjectsEstimation *ObjectsEstimation::get(){

        return ObjectsEstimation::_this;
    }

    bool ObjectsEstimation::init(){

        std::map<std::string, Model*> *models_map = GetModel::getModelMap();

        Model   *model  = nullptr;
        Object  *object = nullptr;

        std::string error = "";
        std::string hex = "";

        std::vector<ObjectData> ods;

        std::vector<Object*> *objects;


        JLC_Compiler  compiler;
        ByteCode *bc = nullptr;

        for(auto &&pair: *models_map){

            model = pair.second;

            if ( !  (object = model->getObject(HelperDASSSRV::T_SCRIPTS_SYS) ) ) {

                continue;
            }



            cDebug()<<"model = " << model->getId();
            object = model->getObject(HelperDASSSRV::T_SCRIPTS_SYS);
            ods = object->getData(error);

            if( ! error.empty() ){

               LOGGER->error(error.c_str() );
               return false;
            }



            for (std::vector<ObjectData>::reverse_iterator r_it = ods.rbegin();
                                                           r_it < ods.rend(); ++r_it) {

                if( ! strToBool((*r_it).getValue("assess_flag") ) ){

                    continue;
                }

                try {



                    hex = (*r_it).getValue("script");

                    if(hex.empty() ){

                        continue;

                        LOGGER->error(_("For URI '%s' script empty"), (*r_it).getId() );
                    }

                    if(hex.front() == '\\'){

                        hex.erase(0,1);

                        if(hex.front() == 'x'){

                            hex.erase(0,1);
                        }
                    }


                    bc = compiler.compiler(Base64::decode( StrUtils::hexToString(hex) ) );


//                    bc = compiler.compiler("var a = 100;\n"
//                                           "alert(object.getId());\n"
//                                           "alert(object.getURI());\n"
//                                           "alert(object.getName());\n"
//                                           "var state = rand()%10;"
//                                           "setVal(\"state\",state);\n"
//                                           "alert(state, getVal(\"state\"), object.getVal(\"state\") );\n"
//                                           "alert(\"+++++++Childs++++++\");\n"
//                                           "for(var item in  object.getChilds()){\n"
//                                           "    alert(\"\t\t\"+item.getURI() );\n"
//                                           "    var state_ =  item.getVal(\"state\") ;\n"
//                                           "    alert(state_ );\n"
//                                           "}\n"
//                                           "alert(\"------Childs-----\");\n"

//                                           /*"alert(object.getAttrs());\n"
//                                           "for(var item in object.getAttrs() ){\n"
//                                           "    alert(object.getValueAttr(item[0] ) );"
//                                           "}\n"
//                                           "setVal(\"state\",0);\n"
//                                           "setVal(\"vltage\",45);\n"
//                                           "setVal(\"current\",0.56);\n"
//                                           "setVal(\"currTime\",currTime);\n"
//                                           "setVal(\"msg\",\"Test+++++++\");\n"*/);

                    _eo_map.insert(std::pair<std::string, EOPtr>((*r_it).getValue("uri"),
                                                                 new EstimationObject((*r_it).getValue("uri"),
                                                                                      bc,
                                                                                      &_vm) ) );
                    //cDebug()<<Base64::decode( StrUtils::hexToString(hex) );

                } catch (std::string error) {

                    LOGGER->error(_("For URI '%s' erorr"), error.c_str());
                }

            }


            objects = model->getObjects();

            for (auto &&object : *objects) {

                if(_eo_map.count(object->getId() ) ){

                    _eo_map.erase(_eo_map.find(object->getId() ) );
                }
            }
        }


        return true;;
    }

    bool ObjectsEstimation::reinit(){

        AutoMutex *am = new AutoMutex(_oe_mutex);

        this->stop();
        _eo_map.clear();

        if( ! this->init() ){

            return false;
        }

        delete am;

        this->start();

        return true;
    }

    void ObjectsEstimation::start(){

        this->execEstimation();
        _oe_timer->start(15);
    }

    void ObjectsEstimation::stop(){

        _oe_timer->stop();
    }


    std::map<std::string, ValuePtr> ObjectsEstimation::getValuesByURI(const std::string &uri){

        if(_eo_map.count(uri)){

            return _eo_map.find(uri)->second->values();
        }

        return std::map<std::string, ValuePtr>();
    }


    ObjectsEstimation::~ObjectsEstimation(){

        delete _oe_timer;
    }

    void ObjectsEstimation::execEstimation(){

        AutoMutex am(_oe_mutex);
        UNUSED(am);


        for (auto &&pair : _eo_map) {

            cDebug()<<"Estimation for =  "<< pair.first;
            try {

                pair.second->execEstimation();

                std::map<std::string, ValuePtr>  values = pair.second->values();

//                for(auto &&val: values){

//                    cDebug()<<val.first <<" " << HelperDASSSRV::ValuePtrToStr(


//                                  val.second ) ;
//                }

            } catch (std::string error) {

                LOGGER->error(_("Error when performing the evaluation object with uri '%s' : %s"),pair.first.c_str(),
                                                                                                  error.c_str() );
            }
        }

        return;
    }
//end  ObjectsEstimation


//begin EstimationObject
    EstimationObject::EstimationObject(const std::string &uri, ByteCode *bc,
                                       JLC_VirtualMachine *vm):
        _uri(uri), _bc(bc), _vm(vm){
    }

    void EstimationObject::execEstimation()throw (std::string){


        ObjectData *od = ObjectData::fromId(_uri);

        if ( !od  || ! od->isValid()) {

            std::stringstream ss;

            ss<<_("Error creating an object 'ObjectData' for uri '")
             <<_uri << "'";

            throw ss.str();
        }


        SaveValFunction *saveValFunction = new SaveValFunction(this);
        GetValFunction  *getValFunction  = new GetValFunction(this);


        time_t rawtime;
        struct tm * timeinfo;
        char buffer[80];

        time (&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer,sizeof(buffer),"%d.%m.%Y %H:%M:%S",timeinfo);
        std::string str(buffer);



        VarDict inputVarDict(NULL);

        inputVarDict.init("object");
        inputVarDict.replace("object", ValuePtr ( new InstanceValue(od) ) );


        inputVarDict.init("currTime");
        inputVarDict.replace("currTime", ValuePtr ( new StrValue(str)  ) );




        VarStack varStack;
        FunDict funDict(nullptr);

        funDict.setFunction("setVal", saveValFunction);
        funDict.setFunction("getVal", getValFunction);

        try {

            _vm->exec(_bc,
                      &varStack,
                      &inputVarDict, &funDict);


            this->setValue("date", ValuePtr ( new StrValue(str) ) );

        } catch (std::string error) {

            this->setValue("date", ValuePtr ( new StrValue(str) ) );
            this->setValue("state", ValuePtr ( new StrValue("-1") ) );
            this->setValue("error", ValuePtr ( new StrValue(error) ) );

            throw error;
        }


        for(auto &&pair: this->values() ){


            HelperDASSSRV::saveVal(pair.first,
                                   HelperDASSSRV::ValuePtrToStr(pair.second),
                                   od);
        }


        return;
    }



    void EstimationObject::setValue(const std::string &key, const ValuePtr &value){

        AutoMutex am(_eo_mutex);
        UNUSED(am);

        if (_vals.count(key) ) {

            _vals.erase(_vals.find(key) );
        }

        _vals.insert(std::pair<std::string, ValuePtr>(key,
                                                      value) );
    }


    std::map<std::string, ValuePtr> EstimationObject::values(){

        AutoMutex am(_eo_mutex);
        UNUSED(am);

        return _vals;
    }

    EstimationObject::~EstimationObject(){

    }


        //begin SaveValFunction
            void EstimationObject::SaveValFunction::exec(VarStack *varStack, VarDict *varDict, FunDict *funDict){

                NOT_USING(varStack);
                NOT_USING(funDict);

                if ( ! varDict->check("arguments") ){

                       VND("arguments");
                }

                ValuePtr argsPtr = varDict->find("arguments");

                std::vector<ValuePtr>  *array = toArray(argsPtr);
                if(array->size() != 2 ){

                    varStack->push(ValuePtr(new  NoNEValue));
                    return;
                }

                std::string key = toStr(array->at(0) );


                ValuePtr valPtr = array->at(1);


                _eo->setValue(key, valPtr );
                return;
            }


        //end SaveValFunction



        //begin GetValFunction
            void EstimationObject::GetValFunction::exec(VarStack *varStack, VarDict *varDict, FunDict *funDict){

                NOT_USING(varStack);
                NOT_USING(funDict);

                if ( ! varDict->check("arguments") ){

                       VND("arguments");
                }


                ValuePtr argsPtr = varDict->find("arguments");


                std::vector<ValuePtr>  *array = toArray(argsPtr);



                if(array->size() == 0 ){

                    varStack->push(ValuePtr(new  NoNEValue));
                    return;
                }


                std::map<std::string, ValuePtr> vals = _eo->values();
                if(array->size() == 1){

                    std::string  key = toStr(array->front() );

                    if(vals.count(key) ){

                        varStack->push( vals.find(key)->second ) ;

                    }else{

                        varStack->push(ValuePtr(new  NoNEValue) );

                    }


                    return;
                }


                ArrayValue *result = new ArrayValue;

                std::vector<ValuePtr> v;
                for (auto &&pair : *array) {

                    std::string  key = toStr(pair );

                    if(vals.count(key) ){

                        v.push_back(vals.find(key)->second );
                    }else{

                        v.push_back(ValuePtr ( new  NoNEValue ) );
                    }

                }

                result->push(v);
                varStack->push(ValuePtr(result));
            }
        //begin GetValFunction




        //begin InstanceValue

        inline void getChildsFromGroup(std::vector<Hierarchy*> *childs, std::set<Object*> &v ){


            for(auto &&child: *childs){


                if(child->getType()== Hierarchy::HIERARHLY_TYPE_OBJECT){

                      v.insert(child->getObjectPtr() );

                }else if(child->getType()== Hierarchy::HIERARHLY_TYPE_GROUP)  {

                        getChildsFromGroup(child->getChilds(), v);
                }
            }

            return;
        }
        inline void fillchildsVector(std::vector<Hierarchy*> *childs, std::set<Object*> &v,
                                     Object  *object){

            for(auto &&child: *childs){



                if(object->getId() == child->getObject() ){

                    //cDebug()<<"\t"<<child->getObject();

                    for(auto &&objectChild: *child->getChilds()){


                        if(objectChild->getType()== Hierarchy::HIERARHLY_TYPE_OBJECT){

                              v.insert(objectChild->getObjectPtr() );

                        }else if(objectChild->getType()== Hierarchy::HIERARHLY_TYPE_GROUP)  {

                                getChildsFromGroup(objectChild->getChilds(), v);
                        }
                    }
                }


                fillchildsVector(child->getChilds(),v, object);
            }

            return;
        }


        EstimationObject::InstanceValue::InstanceValue(ObjectData *od):Value(Value::OBJECT),
            _od(od), _object(_od->getObject()){


            fillchildsVector(_object->getModel()->getHierarchy()->getChilds(),
                             _childs,
                             _object);


//             cDebug()<<_object->getId() << "  " << _object->getName();
//            for(auto &&child: _childs){

//                cDebug()<<"\t"<<child->getId() << "  " << child->getName();
//            }

            REG_MTD(InstanceValue, getId);
            REG_MTD(InstanceValue, getURI);
            REG_MTD(InstanceValue, getName);
            REG_MTD(InstanceValue, getAttrs);
            REG_MTD(InstanceValue, getChilds);
            REG_MTD(InstanceValue, getValueAttr);

            REG_MTD(InstanceValue, getVal);

        }

        Value *EstimationObject::InstanceValue::mls(const Value &value){

            NOT_USING(value);
            return new NoNEValue;
        }


        bool EstimationObject::InstanceValue::equal(const Value &v){

            NOT_USING(v);
            return false;
        }


        ValuePtr EstimationObject::InstanceValue::getId(std::vector<ValuePtr> &v){

            NOT_USING(v);
            return ValuePtr(new StrValue(_object->getId() ) );
        }

        ValuePtr EstimationObject::InstanceValue::getURI(std::vector<ValuePtr> &v){

            NOT_USING(v);
            return ValuePtr(new StrValue(_od->getId() ) );
        }


        ValuePtr EstimationObject::InstanceValue::getName(std::vector<ValuePtr> &v){

            NOT_USING(v);
            return ValuePtr(new StrValue(_object->getName() ) );
        }

        ValuePtr EstimationObject::InstanceValue::getAttrs(std::vector<ValuePtr> &v){

            NOT_USING(v);

            ArrayValue *attrs = new  ArrayValue();

            ArrayValue *pair = nullptr;
            for (auto &&attr: *_object->getAttrs()) {

                pair = new ArrayValue();

                pair->add(new StrValue(attr->getId() ) );
                pair->add(new StrValue(attr->getName() ) );

                attrs->add(pair);
            }

            return ValuePtr(attrs);
        }

        ValuePtr EstimationObject::InstanceValue::getChilds(std::vector<ValuePtr> &v){

            NOT_USING(v);

            ArrayValue *array = new  ArrayValue();

            for(auto &&child: _childs){




                for(auto &&rel: *child->getRelations() ){




                    if(_object->getId() != rel->getToObject()->getId()){

                        continue;
                    }


                    std::vector<std::string > key;
                    std::vector<std::string > value;

                    for(auto &&ft : *rel->getFromTo()){

                        sRel from = ft.from;
                        sRel to = ft.to;


                        key.push_back(from.attr->getId() );
                        value.push_back(_od->getValue(to.attr->getId() ) );

                    }



                    std::string                     error = "";
                    std::vector<std::vector<Cell> > result;
                    GetGPQConn::get()->getData(child->getModel()->getId(),
                                               child->getId(),
                                               std::vector<std::string>(),
                                               key,value,
                                               result,
                                               error);

                    if(! error.empty() ){

                        throw error;
                    }

                    cDebug()<<child->getId();

                    for(auto &&row: result){

//                        for(auto &&cell: row){

//                            cDebug()<<cell.id;
//                        }
                        ObjectData *ob_ =new ObjectData(row, child);

                        if(! ob_->isValid() ){

                            std::stringstream ss;

                            ss<<_("Error getting child elements for an object with '")
                             << _od->getId() << "'";
                            throw ss.str();
                        }

                        array->add( new InstanceValue(ob_));
                        //cDebug()<<"\t = "<<Base64::decode(ob_.getId() );
                    }


//                    for (int i = 0; i < key.size(); ++i) {

//                        cDebug()<<"\t\t_object = " << _object->getId()
//                               <<"  "<<key.at(i) << "   "<<value.at(i);
//                    }
                }

            }

            return ValuePtr(array);
        }

        ValuePtr EstimationObject::InstanceValue::getValueAttr(std::vector<ValuePtr> &v){

            if(v.empty() ){

                return ValuePtr(new NoNEValue() );
            }

            return ValuePtr(new StrValue(_od->getValue(toStr(v.front() ) ) ) );
        }

        ValuePtr EstimationObject::InstanceValue::getVal(std::vector<ValuePtr> &v){


            std::map<std::string, ValuePtr>  vals = OSE->getValuesByURI(_od->getId() );

            if(v.size() == 0  || vals.size() == 0){

                return ValuePtr(new  NoNEValue);
            }

            if(v.size() == 1){

                std::string  key = toStr(v.front() );

                if(vals.count(key) ){

                    return vals.find(key)->second ;

                }else{

                    return ValuePtr(new  NoNEValue);
                }

            }


            ArrayValue *result = new ArrayValue;


            for (auto &&pair : v) {

                std::string  key = toStr(pair );
                std::vector<ValuePtr> v;

                if(vals.count(key) ){

                    v.push_back(vals.find(key)->second );

                }else{

                    v.push_back(ValuePtr(new  NoNEValue ) );
                }

            }

            result->push(v);


            return ValuePtr(result);
        }

    //end InstanceValue

//end EstimationObject



