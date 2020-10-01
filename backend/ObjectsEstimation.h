#ifndef OBJECTSESTIMATION_H
#define OBJECTSESTIMATION_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <signal.h>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <string>


#include "./HelperDASSSRV.h"


#include "./utils/Sloth.h"
#include "./utils/Timer.h"


#define OSE ObjectsEstimation::get()

class EstimationObject;
typedef std::shared_ptr<EstimationObject > EOPtr;

/**
 * @brief ObjectsEstimation             - класс оценки объектов
 */
class ObjectsEstimation
{
public:

    class init_{
        public:
            init_();
            ~init_();
    };

    static ObjectsEstimation *get();

    /**
     * @brief init                      - инициализация
     * @return                          - true в случае успешной инициализации
     */
    bool init();



    /**
     * @brief reinit                    - переинициализация
     * @return                          - true в случае успешной инициализации
     */
    bool reinit();


    /**
     * @brief start                     - запуск оценки объектов
     */
    void start();


    /**
     * @brief stop                      - остановка оценки объектов
     */
    void stop();


    /// получить знания по uri значение : uri значения
    std::map<std::string, ValuePtr> getValuesByURI(const std::string &uri);

    /**
     * @brief ~ObjectsEstimation        - деструктор
     */
    ~ObjectsEstimation();

private:

    friend class init;

    static ObjectsEstimation * _this;
    std::map<std::string, EOPtr>         _eo_map;    ///< словарь обектов оценки, где ключ - uri, а значение - объект оценки
    JLC_VirtualMachine                   _vm;        ///< виртуальная машина языка 'JLC'

    Mutex                _oe_mutex;

    Timer<ObjectsEstimation*, void(ObjectsEstimation::*)()>    *_oe_timer;                ///< таймер оценки объектов



    /**
     * @brief ObjectsEstimation         - конструктор без параметров
     */
    ObjectsEstimation();


    /**
     * @brief execEstimation            - выполнить оценку
     */
    void execEstimation();
};


const static ObjectsEstimation::init_ init;

/**
 * @brief EstimationObject              - объект оценки
 */
class EstimationObject
{
public:

    /**
     * @brief EstimationObject          - конструктор
     * @param uri                       - уникальный идентификатор объекта
     * @param bc                        - байт код оценки объекта
     * @param vm                        - виртуальная машина языка 'JLC'
     */
    EstimationObject(const std::string &uri, ByteCode *bc,
                     JLC_VirtualMachine  *vm);


    /**
     * @brief execEstimation            - выполнить оценку
     * @throw std::string               - строка с ошибкой
     */
    void execEstimation()throw( std::string);


    /**
     * @brief setValue                  - cохраненить значений вычисленных скриптом
     * @param key                       - строка содержащая наименование значение
     * @param value                     - строка содержащая значение
     */
    void setValue(const std::string &key, const ValuePtr &value);


    /**
     * @brief values                    - возвращает значения вычисленных скриптом
     * @return                          - словарь значений вычисленных скриптом
     */
    std::map<std::string, ValuePtr> values();



    ~EstimationObject();

private:

    std::string          _uri;               ///< уникальный идентификатор объекта
    ByteCode            *_bc;                ///< байт код оценки объекта
    JLC_VirtualMachine  *_vm;                ///< виртуальная машина языка 'JLC'

    Mutex                _eo_mutex;

    std::map<std::string, ValuePtr>  _vals;      ///<  словарь значений вычисленных скриптом


    /**
     * @brief SaveValFunction           - cохранение значений вычисленных скриптом
     */
    class SaveValFunction :  public JLC_Function{

        public:
            SaveValFunction(EstimationObject *eo):_eo(eo) {}

            virtual void exec(VarStack *varStack, VarDict *varDict,
                              FunDict *funDict);

        private:

            EstimationObject *_eo;
    };


    /**
     * @brief SaveValFunction           -  получение сохранёных значений вычисленных скриптом
     */
    class GetValFunction :  public JLC_Function{

        public:
            GetValFunction(EstimationObject *eo):_eo(eo) {}

            virtual void exec(VarStack *varStack, VarDict *varDict,
                              FunDict *funDict);

        private:

            EstimationObject *_eo;
    };


    class InstanceValue: public Value{

        public:
            InstanceValue(ObjectData *od);

            virtual Value *mls(const Value &value) ;
            bool equal(const Value &v);

        private:

            ObjectData  *_od;
            Object      *_object;

            std::set<Object*> _childs;                       ///< вектор дочерних элементов
            std::map<std::string, std::string>  *_vals;      ///<  словарь значений вычисленных скриптом


            ///< получить индетификатор объекта(как в онтологии 'id')
            ValuePtr getId(std::vector<ValuePtr> &v);

            ///< уникальный идентификатор записи экземпляра
            ValuePtr getURI(std::vector<ValuePtr> &v);

            ///< получить наименование объекта(как в онтологии 'name')
            ValuePtr getName(std::vector<ValuePtr> &v);

            ///< получить список атрибутов объекта
            ValuePtr getAttrs(std::vector<ValuePtr> &v);

            ///< получить список получить список дочерних экземпляров
            ValuePtr getChilds(std::vector<ValuePtr> &v);

            ///< получить получить значение атрибута
            ValuePtr getValueAttr(std::vector<ValuePtr> &v);

            /// получение сохранёных значений вычисленных скриптом
            ValuePtr getVal(std::vector<ValuePtr> &v);

    };

};


#endif // OBJECTSESTIMATION_H
