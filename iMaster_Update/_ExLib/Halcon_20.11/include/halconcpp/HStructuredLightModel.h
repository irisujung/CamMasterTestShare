/***********************************************************
 * File generated by the HALCON-Compiler hcomp version 20.11
 * Usage: Interface to C++
 *
 * Software by: MVTec Software GmbH, www.mvtec.com
 ***********************************************************/


#ifndef HCPP_HSTRUCTUREDLIGHTMODEL
#define HCPP_HSTRUCTUREDLIGHTMODEL

namespace HalconCpp
{

// Represents an instance of a structured light model.
class LIntExport HStructuredLightModel : public HHandle
{

public:

  // Create an uninitialized instance
  HStructuredLightModel():HHandle() {}

  // Copy constructor
  HStructuredLightModel(const HStructuredLightModel& source) : HHandle(source) {}

  // Copy constructor
  HStructuredLightModel(const HHandle& handle);

  // Create HStructuredLightModel from handle, taking ownership
  explicit HStructuredLightModel(Hlong handle);

  bool operator==(const HHandle& obj) const
  {
    return HHandleBase::operator==(obj);
  }

  bool operator!=(const HHandle& obj) const
  {
    return HHandleBase::operator!=(obj);
  }

protected:

  // Verify matching semantic type ('structured_light_model')!
  virtual void AssertType(Hphandle handle) const;

public:

  // Deep copy of all data represented by this object instance
  HStructuredLightModel Clone() const;



/*****************************************************************************
 * Operator-based class constructors
 *****************************************************************************/

  // create_structured_light_model: Create a structured light model.
  explicit HStructuredLightModel(const HString& ModelType);

  // create_structured_light_model: Create a structured light model.
  explicit HStructuredLightModel(const char* ModelType);

#ifdef _WIN32
  // create_structured_light_model: Create a structured light model.
  explicit HStructuredLightModel(const wchar_t* ModelType);
#endif




  /***************************************************************************
   * Operators                                                               *
   ***************************************************************************/

  // Clear a structured light model and free the allocated memory.
  static void ClearStructuredLightModel(const HStructuredLightModelArray& StructuredLightModel);

  // Clear a structured light model and free the allocated memory.
  void ClearStructuredLightModel() const;

  // Create a structured light model.
  void CreateStructuredLightModel(const HString& ModelType);

  // Create a structured light model.
  void CreateStructuredLightModel(const char* ModelType);

#ifdef _WIN32
  // Create a structured light model.
  void CreateStructuredLightModel(const wchar_t* ModelType);
#endif

  // Decode the camera images acquired with a structured light setup.
  void DecodeStructuredLightPattern(const HImage& CameraImages) const;

  // Deserialize a structured light model.
  void DeserializeStructuredLightModel(const HSerializedItem& SerializedItemHandle);

  // Generate the pattern images to be displayed in a structured light setup.
  HImage GenStructuredLightPattern() const;

  // Query parameters of a structured light model.
  HTuple GetStructuredLightModelParam(const HTuple& GenParamName) const;

  // Query parameters of a structured light model.
  HTuple GetStructuredLightModelParam(const HString& GenParamName) const;

  // Query parameters of a structured light model.
  HTuple GetStructuredLightModelParam(const char* GenParamName) const;

#ifdef _WIN32
  // Query parameters of a structured light model.
  HTuple GetStructuredLightModelParam(const wchar_t* GenParamName) const;
#endif

  // Get (intermediate) iconic results of a structured light model.
  HObject GetStructuredLightObject(const HTuple& ObjectName) const;

  // Get (intermediate) iconic results of a structured light model.
  HObject GetStructuredLightObject(const HString& ObjectName) const;

  // Get (intermediate) iconic results of a structured light model.
  HObject GetStructuredLightObject(const char* ObjectName) const;

#ifdef _WIN32
  // Get (intermediate) iconic results of a structured light model.
  HObject GetStructuredLightObject(const wchar_t* ObjectName) const;
#endif

  // Read a structured light model from a file.
  void ReadStructuredLightModel(const HString& FileName);

  // Read a structured light model from a file.
  void ReadStructuredLightModel(const char* FileName);

#ifdef _WIN32
  // Read a structured light model from a file.
  void ReadStructuredLightModel(const wchar_t* FileName);
#endif

  // Serialize a structured light model.
  HSerializedItem SerializeStructuredLightModel() const;

  // Set parameters of a structured light model.
  void SetStructuredLightModelParam(const HTuple& GenParamName, const HTuple& GenParamValue) const;

  // Set parameters of a structured light model.
  void SetStructuredLightModelParam(const HString& GenParamName, Hlong GenParamValue) const;

  // Set parameters of a structured light model.
  void SetStructuredLightModelParam(const char* GenParamName, Hlong GenParamValue) const;

#ifdef _WIN32
  // Set parameters of a structured light model.
  void SetStructuredLightModelParam(const wchar_t* GenParamName, Hlong GenParamValue) const;
#endif

  // Write a structured light model to a file.
  void WriteStructuredLightModel(const HString& FileName) const;

  // Write a structured light model to a file.
  void WriteStructuredLightModel(const char* FileName) const;

#ifdef _WIN32
  // Write a structured light model to a file.
  void WriteStructuredLightModel(const wchar_t* FileName) const;
#endif

};

// forward declarations and types for internal array implementation

template<class T> class HSmartPtr;
template<class T> class HHandleBaseArrayRef;

typedef HHandleBaseArrayRef<HStructuredLightModel> HStructuredLightModelArrayRef;
typedef HSmartPtr< HStructuredLightModelArrayRef > HStructuredLightModelArrayPtr;


// Represents multiple tool instances
class LIntExport HStructuredLightModelArray : public HHandleBaseArray
{

public:

  // Create empty array
  HStructuredLightModelArray();

  // Create array from native array of tool instances
  HStructuredLightModelArray(HStructuredLightModel* classes, Hlong length);

  // Copy constructor
  HStructuredLightModelArray(const HStructuredLightModelArray &tool_array);

  // Destructor
  virtual ~HStructuredLightModelArray();

  // Assignment operator
  HStructuredLightModelArray &operator=(const HStructuredLightModelArray &tool_array);

  // Clears array and all tool instances
  virtual void Clear();

  // Get array of native tool instances
  const HStructuredLightModel* Tools() const;

  // Get number of tools
  virtual Hlong Length() const;

  // Create tool array from tuple of handles
  virtual void SetFromTuple(const HTuple& handles);

  // Get tuple of handles for tool array
  virtual HTuple ConvertToTuple() const;

protected:

// Smart pointer to internal data container
   HStructuredLightModelArrayPtr *mArrayPtr;
};

}

#endif
