// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PxEventDictionary

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "PxEvent.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_PxEvent(void *p = 0);
   static void *newArray_PxEvent(Long_t size, void *p);
   static void delete_PxEvent(void *p);
   static void deleteArray_PxEvent(void *p);
   static void destruct_PxEvent(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PxEvent*)
   {
      ::PxEvent *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PxEvent >(0);
      static ::ROOT::TGenericClassInfo 
         instance("PxEvent", ::PxEvent::Class_Version(), "PxEvent.h", 56,
                  typeid(::PxEvent), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PxEvent::Dictionary, isa_proxy, 4,
                  sizeof(::PxEvent) );
      instance.SetNew(&new_PxEvent);
      instance.SetNewArray(&newArray_PxEvent);
      instance.SetDelete(&delete_PxEvent);
      instance.SetDeleteArray(&deleteArray_PxEvent);
      instance.SetDestructor(&destruct_PxEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PxEvent*)
   {
      return GenerateInitInstanceLocal((::PxEvent*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::PxEvent*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PxEvent::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *PxEvent::Class_Name()
{
   return "PxEvent";
}

//______________________________________________________________________________
const char *PxEvent::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PxEvent*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int PxEvent::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PxEvent*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PxEvent::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PxEvent*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PxEvent::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PxEvent*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PxEvent::Streamer(TBuffer &R__b)
{
   // Stream an object of class PxEvent.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PxEvent::Class(),this);
   } else {
      R__b.WriteClassBuffer(PxEvent::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PxEvent(void *p) {
      return  p ? new(p) ::PxEvent : new ::PxEvent;
   }
   static void *newArray_PxEvent(Long_t nElements, void *p) {
      return p ? new(p) ::PxEvent[nElements] : new ::PxEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_PxEvent(void *p) {
      delete ((::PxEvent*)p);
   }
   static void deleteArray_PxEvent(void *p) {
      delete [] ((::PxEvent*)p);
   }
   static void destruct_PxEvent(void *p) {
      typedef ::PxEvent current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PxEvent

namespace ROOT {
   static TClass *vectorlEunsignedsPintgR_Dictionary();
   static void vectorlEunsignedsPintgR_TClassManip(TClass*);
   static void *new_vectorlEunsignedsPintgR(void *p = 0);
   static void *newArray_vectorlEunsignedsPintgR(Long_t size, void *p);
   static void delete_vectorlEunsignedsPintgR(void *p);
   static void deleteArray_vectorlEunsignedsPintgR(void *p);
   static void destruct_vectorlEunsignedsPintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<unsigned int>*)
   {
      vector<unsigned int> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<unsigned int>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<unsigned int>", -2, "vector", 210,
                  typeid(vector<unsigned int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEunsignedsPintgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<unsigned int>) );
      instance.SetNew(&new_vectorlEunsignedsPintgR);
      instance.SetNewArray(&newArray_vectorlEunsignedsPintgR);
      instance.SetDelete(&delete_vectorlEunsignedsPintgR);
      instance.SetDeleteArray(&deleteArray_vectorlEunsignedsPintgR);
      instance.SetDestructor(&destruct_vectorlEunsignedsPintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<unsigned int> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<unsigned int>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEunsignedsPintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<unsigned int>*)0x0)->GetClass();
      vectorlEunsignedsPintgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEunsignedsPintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEunsignedsPintgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<unsigned int> : new vector<unsigned int>;
   }
   static void *newArray_vectorlEunsignedsPintgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<unsigned int>[nElements] : new vector<unsigned int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEunsignedsPintgR(void *p) {
      delete ((vector<unsigned int>*)p);
   }
   static void deleteArray_vectorlEunsignedsPintgR(void *p) {
      delete [] ((vector<unsigned int>*)p);
   }
   static void destruct_vectorlEunsignedsPintgR(void *p) {
      typedef vector<unsigned int> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<unsigned int>

namespace ROOT {
   static TClass *vectorlEPxDecodedHitgR_Dictionary();
   static void vectorlEPxDecodedHitgR_TClassManip(TClass*);
   static void *new_vectorlEPxDecodedHitgR(void *p = 0);
   static void *newArray_vectorlEPxDecodedHitgR(Long_t size, void *p);
   static void delete_vectorlEPxDecodedHitgR(void *p);
   static void deleteArray_vectorlEPxDecodedHitgR(void *p);
   static void destruct_vectorlEPxDecodedHitgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<PxDecodedHit>*)
   {
      vector<PxDecodedHit> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<PxDecodedHit>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<PxDecodedHit>", -2, "vector", 210,
                  typeid(vector<PxDecodedHit>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEPxDecodedHitgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<PxDecodedHit>) );
      instance.SetNew(&new_vectorlEPxDecodedHitgR);
      instance.SetNewArray(&newArray_vectorlEPxDecodedHitgR);
      instance.SetDelete(&delete_vectorlEPxDecodedHitgR);
      instance.SetDeleteArray(&deleteArray_vectorlEPxDecodedHitgR);
      instance.SetDestructor(&destruct_vectorlEPxDecodedHitgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<PxDecodedHit> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<PxDecodedHit>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEPxDecodedHitgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<PxDecodedHit>*)0x0)->GetClass();
      vectorlEPxDecodedHitgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEPxDecodedHitgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEPxDecodedHitgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PxDecodedHit> : new vector<PxDecodedHit>;
   }
   static void *newArray_vectorlEPxDecodedHitgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PxDecodedHit>[nElements] : new vector<PxDecodedHit>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEPxDecodedHitgR(void *p) {
      delete ((vector<PxDecodedHit>*)p);
   }
   static void deleteArray_vectorlEPxDecodedHitgR(void *p) {
      delete [] ((vector<PxDecodedHit>*)p);
   }
   static void destruct_vectorlEPxDecodedHitgR(void *p) {
      typedef vector<PxDecodedHit> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<PxDecodedHit>

namespace {
  void TriggerDictionaryInitialization_PxEventDictionary_Impl() {
    static const char* headers[] = {
"PxEvent.h",
0
    };
    static const char* includePaths[] = {
"/opt/local/root_v6.06.08-qt_v5.9.1-g++v4.8.5/include",
"/home/cmstestbeam/Programming/Monicelli/templates/inc/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "PxEventDictionary dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$PxEvent.h")))  PxEvent;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "PxEventDictionary dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "PxEvent.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"PxEvent", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("PxEventDictionary",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_PxEventDictionary_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_PxEventDictionary_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_PxEventDictionary() {
  TriggerDictionaryInitialization_PxEventDictionary_Impl();
}
