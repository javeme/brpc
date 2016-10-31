#pragma once
#include "orm/Condition.h"
#include "orm/Types.h"
#include "src/util/BRpcUtil.h"


namespace brpc{

/////////////////////////////////////////////////////////////////
//field value with type info
template <typename T/*, T deftVal=T()*/>
class TypeField : public blib::Field<T>
{
public:
    TypeField() : m_fieldInfo(null) {}
    TypeField(const T& val) : Field<T>(val) {}
    virtual ~TypeField() {}
public:
    virtual TypeField& operator=(const T& val) {
        Field<T>::operator=(val);
        return *this;
    }

    virtual const FieldInfo* getType() const { return m_fieldInfo; }
    virtual void setType(const FieldInfo* val) { m_fieldInfo = val; }

public:
    virtual cstring fieldName() const {
        return m_fieldInfo ? m_fieldInfo->name() : "";
    }

private:
    const FieldInfo* m_fieldInfo;
};


//Column Model
class ColumnModel : public Object
{
public:
    ColumnModel(cstring name="", bool primary=false, bool unique=false,
        bool notNull=false, cstring default="", bool store=true)
        : m_name(name), m_primary(primary), m_unique(unique),
        m_notNull(notNull), m_default(default), m_needStore(store) {}
public:
    virtual void setColumnName(cstring str) { m_name = str; }
    virtual String columnName() const { return m_name; }

    virtual String defaultValue() const { return m_default; }
    virtual void setDefaultValue(cstring val) { m_default = val; }

    virtual bool isPrimary() const { return m_primary; }
    virtual void setPrimary(bool val) { m_primary = val; }

    virtual bool isUnique() const { return m_unique; }
    virtual void setUnique(bool val) { m_unique = val; }

    virtual bool isNotNull() const { return m_notNull; }
    virtual void setNotNull(bool val) { m_notNull = val; }

    virtual bool isNeedStore() const { return m_needStore; }
    virtual void setNeedStore(bool val) { m_needStore = val; }
protected:
    String m_name;
    String m_default;
    bool m_primary;
    bool m_unique;
    bool m_notNull;

    bool m_needStore;
};

class Column : public Object
{
public:
    Column(ColumnModel* model=null) : m_model(model) {}

    virtual ColumnModel* getModel() const { return m_model; }
    virtual void setModel(ColumnModel* val) {
        this->m_model = val;
        checkNullPtr(this->m_model);
    }
public:
    virtual void setColumnName(cstring str) {
        checkNullPtr(this->m_model);
        m_model->setColumnName(str);
    }
    virtual String columnName() const {
        checkNullPtr(this->m_model);
        return m_model->columnName();
    }

    virtual String defaultValue() const {
        checkNullPtr(this->m_model);
        return m_model->defaultValue();
    }
    virtual void setDefaultValue(cstring val) {
        checkNullPtr(this->m_model);
        m_model->setDefaultValue(val);
    }

    virtual bool isPrimary() const {
        checkNullPtr(this->m_model);
        return m_model->isPrimary();
    }
    virtual void setPrimary(bool val) {
        checkNullPtr(this->m_model);
        m_model->setPrimary(val);
    }

    virtual bool isUnique() const {
        checkNullPtr(this->m_model);
        return m_model->isUnique();
    }
    virtual void setUnique(bool val) {
        checkNullPtr(this->m_model);
        m_model->setUnique(val);
    }

    virtual bool isNotNull() const {
        checkNullPtr(this->m_model);
        return m_model->isNotNull();
    }
    virtual void setNotNull(bool val) {
        checkNullPtr(this->m_model);
        m_model->setNotNull(val);
    }

    virtual bool isNeedStore() const {
        checkNullPtr(this->m_model);
        return m_model->isNeedStore();
    }
    virtual void setNeedStore(bool val) {
        checkNullPtr(this->m_model);
        m_model->setNeedStore(val);
    }
public:
    virtual ConditionWrapper query() const = 0;
    virtual ConditionWrapper group() const = 0;

    virtual ConditionWrapper columnValue() const = 0;

    virtual String field() const = 0;
    virtual String columnType() const = 0;
private:
    ColumnModel* m_model;
};

template <typename T>
class TypeColumn : public Column, public TypeField<T>
{
public:
    TypeColumn() : Column(), TypeField<T>() {}
    TypeColumn(const T& val) : Column(), TypeField<T>(val) {}
public:
    virtual TypeColumn& operator=(const T& val) {
        TypeField<T>::operator=(val);
        return *this;
    }

    virtual String toString() const {
        return Value2String<T>((T)*this);
    }

public:
    virtual ConditionWrapper query() const {
        if (this->isNeedStore()) {
            return new FieldCondition(this->columnName());
        }
        else {
            // like count(*)
            return new ValueCondition(this->columnName());
        }
    }
    virtual ConditionWrapper group() const {
        return new FieldCondition(this->fieldName());
    }

    virtual String columnName() const {
        String col = Column::columnName();
        return col.length() ? col : fieldName();
    }
    virtual ConditionWrapper columnValue() const {
        //return Value2String<T>(*this);
        const bool convertible = is_convertible<T, String>::value;
        return new ValueCondition(
            string_caster<T, convertible>::toString(*this));
    }

    virtual String field() const {
        return fieldName();
    }
    virtual String columnType() const {
        const bool convertible = is_convertible<T, dbtype>::value;
        return typename_getter<T, convertible>::typeName(value());
    }
};


//define table name
#define TABLE(table) \
    static cstring tableName() { return #table; }                             \
    virtual String getTableName() const { return tableName(); }               \
/*end of TABLE*/

//define column register method
#define COLUME_REG(name, col, primary, unique, notNull, deflt, store)         \
    class InerClassForRegField_##name                                         \
    {                                                                         \
    public:                                                                   \
        InerClassForRegField_##name(){                                        \
            Self& parent = *(Self*)((char*)this - offsetof(Self,              \
                _instanceOfInerClassForRegField_##name));                     \
            static bool notReged = true;                                      \
            static ColumnModel model(col, primary, unique,                    \
                notNull, deflt, store);                                       \
            if(notReged) {                                                    \
                regField(#name, &Self::name);                                 \
                notReged = false;                                             \
            }                                                                 \
            parent.name.setType(Self::thisClass()->getField(#name));          \
            parent.name.setModel(&model);                                     \
        }                                                                     \
    }_instanceOfInerClassForRegField_##name;                                  \
/*end of COLUME_REG*/

//define a db column(field)
#define COLUME8(name, type, colName, primary, unique, notNull, deflt, store)  \
    TypeColumn<type> name;                                                    \
    COLUME_REG(name, colName, primary, unique, notNull, deflt, store)         \
/*end of COLUME8*/

#define COLUME_P(name, type, colName, primary)                                \
    COLUME8(name, type, colName, primary, false, false, "", true)
#define COLUME_N(name, type, colName) COLUME_P(name, type, colName, false)
#define COLUME_T(name, type) COLUME_N(name, type, "")
#define COLUME_S(name) COLUME_T(name, varchar<32>)
#define COLUME COLUME_T

#define ID(type, colName)                                                     \
    COLUME_N(_id, type, colName);                                             \
    virtual Column& id(){ return _id; }                                       \
/*end of ID*/

}//end of namespace brpc


namespace blib{

//类型转换器(Object* <==> TypeField)
template <typename Type>
struct Converter<brpc::TypeField<Type>>
{
    static inline brpc::TypeField<Type> valueOf(Object* obj)
    {
        brpc::TypeField<Type>* fld = dynamic_cast<brpc::TypeField<Type>*>(obj);
        if(fld)
            return fld->value();
        return Converter<Type>::valueOf(obj);
    }
    static inline Object* toObject(const brpc::TypeField<Type>& val)
    {
        return Converter<Type>::toObject(val.value());
    }
};

//类型转换器(Object* <==> TypeColume)
template <typename Type>
struct Converter<brpc::TypeColumn<Type>>
{
    static inline brpc::TypeColumn<Type> valueOf(Object* obj)
    {
        brpc::TypeColumn<Type>* fld = dynamic_cast<brpc::TypeColumn<Type>*>(obj);
        if(fld)
            return fld->value();
        return Converter<Type>::valueOf(obj);
    }
    static inline Object* toObject(const brpc::TypeColumn<Type>& val)
    {
        return Converter<Type>::toObject(val.value());
    }
};

}//end of namespace blib