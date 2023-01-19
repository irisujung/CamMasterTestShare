/***********************************************************
 * File generated by the HALCON-Compiler hcomp version 20.11
 * Usage: Interface to C++
 *
 * Software by: MVTec Software GmbH, www.mvtec.com
 ***********************************************************/


#ifndef HCPP_HBARCODE
#define HCPP_HBARCODE

namespace HalconCpp
{

// Represents an instance of a bar code reader.
class LIntExport HBarCode : public HHandle
{

public:

  // Create an uninitialized instance
  HBarCode():HHandle() {}

  // Copy constructor
  HBarCode(const HBarCode& source) : HHandle(source) {}

  // Copy constructor
  HBarCode(const HHandle& handle);

  // Create HBarCode from handle, taking ownership
  explicit HBarCode(Hlong handle);

  bool operator==(const HHandle& obj) const
  {
    return HHandleBase::operator==(obj);
  }

  bool operator!=(const HHandle& obj) const
  {
    return HHandleBase::operator!=(obj);
  }

protected:

  // Verify matching semantic type ('barcode')!
  virtual void AssertType(Hphandle handle) const;

public:

  // Deep copy of all data represented by this object instance
  HBarCode Clone() const;



/*****************************************************************************
 * Operator-based class constructors
 *****************************************************************************/

  // read_bar_code_model: Read a bar code model from a file and create a new model.
  explicit HBarCode(const HString& FileName);

  // read_bar_code_model: Read a bar code model from a file and create a new model.
  explicit HBarCode(const char* FileName);

#ifdef _WIN32
  // read_bar_code_model: Read a bar code model from a file and create a new model.
  explicit HBarCode(const wchar_t* FileName);
#endif

  // create_bar_code_model: Create a model of a bar code reader.
  explicit HBarCode(const HTuple& GenParamName, const HTuple& GenParamValue);

  // create_bar_code_model: Create a model of a bar code reader.
  explicit HBarCode(const HString& GenParamName, double GenParamValue);

  // create_bar_code_model: Create a model of a bar code reader.
  explicit HBarCode(const char* GenParamName, double GenParamValue);

#ifdef _WIN32
  // create_bar_code_model: Create a model of a bar code reader.
  explicit HBarCode(const wchar_t* GenParamName, double GenParamValue);
#endif




  /***************************************************************************
   * Operators                                                               *
   ***************************************************************************/

  // Deserialize a bar code model.
  void DeserializeBarCodeModel(const HSerializedItem& SerializedItemHandle);

  // Serialize a bar code model.
  HSerializedItem SerializeBarCodeModel() const;

  // Read a bar code model from a file and create a new model.
  void ReadBarCodeModel(const HString& FileName);

  // Read a bar code model from a file and create a new model.
  void ReadBarCodeModel(const char* FileName);

#ifdef _WIN32
  // Read a bar code model from a file and create a new model.
  void ReadBarCodeModel(const wchar_t* FileName);
#endif

  // Write a bar code model to a file.
  void WriteBarCodeModel(const HString& FileName) const;

  // Write a bar code model to a file.
  void WriteBarCodeModel(const char* FileName) const;

#ifdef _WIN32
  // Write a bar code model to a file.
  void WriteBarCodeModel(const wchar_t* FileName) const;
#endif

  // Access iconic objects that were created during the search or decoding of bar code symbols.
  HObject GetBarCodeObject(const HTuple& CandidateHandle, const HString& ObjectName) const;

  // Access iconic objects that were created during the search or decoding of bar code symbols.
  HObject GetBarCodeObject(const HString& CandidateHandle, const HString& ObjectName) const;

  // Access iconic objects that were created during the search or decoding of bar code symbols.
  HObject GetBarCodeObject(const char* CandidateHandle, const char* ObjectName) const;

#ifdef _WIN32
  // Access iconic objects that were created during the search or decoding of bar code symbols.
  HObject GetBarCodeObject(const wchar_t* CandidateHandle, const wchar_t* ObjectName) const;
#endif

  // Get the alphanumerical results that were accumulated during the decoding of bar code symbols.
  HTuple GetBarCodeResult(const HTuple& CandidateHandle, const HString& ResultName) const;

  // Get the alphanumerical results that were accumulated during the decoding of bar code symbols.
  HTuple GetBarCodeResult(const HString& CandidateHandle, const HString& ResultName) const;

  // Get the alphanumerical results that were accumulated during the decoding of bar code symbols.
  HTuple GetBarCodeResult(const char* CandidateHandle, const char* ResultName) const;

#ifdef _WIN32
  // Get the alphanumerical results that were accumulated during the decoding of bar code symbols.
  HTuple GetBarCodeResult(const wchar_t* CandidateHandle, const wchar_t* ResultName) const;
#endif

  // Decode bar code symbols within a rectangle.
  HTuple DecodeBarCodeRectangle2(const HImage& Image, const HTuple& CodeType, const HTuple& Row, const HTuple& Column, const HTuple& Phi, const HTuple& Length1, const HTuple& Length2) const;

  // Decode bar code symbols within a rectangle.
  HString DecodeBarCodeRectangle2(const HImage& Image, const HString& CodeType, double Row, double Column, double Phi, double Length1, double Length2) const;

  // Decode bar code symbols within a rectangle.
  HString DecodeBarCodeRectangle2(const HImage& Image, const char* CodeType, double Row, double Column, double Phi, double Length1, double Length2) const;

#ifdef _WIN32
  // Decode bar code symbols within a rectangle.
  HString DecodeBarCodeRectangle2(const HImage& Image, const wchar_t* CodeType, double Row, double Column, double Phi, double Length1, double Length2) const;
#endif

  // Detect and read bar code symbols in an image.
  HRegion FindBarCode(const HImage& Image, const HTuple& CodeType, HTuple* DecodedDataStrings) const;

  // Detect and read bar code symbols in an image.
  HRegion FindBarCode(const HImage& Image, const HString& CodeType, HString* DecodedDataStrings) const;

  // Detect and read bar code symbols in an image.
  HRegion FindBarCode(const HImage& Image, const char* CodeType, HString* DecodedDataStrings) const;

#ifdef _WIN32
  // Detect and read bar code symbols in an image.
  HRegion FindBarCode(const HImage& Image, const wchar_t* CodeType, HString* DecodedDataStrings) const;
#endif

  // Get the names of the parameters that can be used in set_bar_code* and get_bar_code* operators for a given bar code model
  HTuple QueryBarCodeParams(const HString& Properties) const;

  // Get the names of the parameters that can be used in set_bar_code* and get_bar_code* operators for a given bar code model
  HTuple QueryBarCodeParams(const char* Properties) const;

#ifdef _WIN32
  // Get the names of the parameters that can be used in set_bar_code* and get_bar_code* operators for a given bar code model
  HTuple QueryBarCodeParams(const wchar_t* Properties) const;
#endif

  // Get parameters that are used by the bar code reader when processing a specific bar code type. 
  HTuple GetBarCodeParamSpecific(const HTuple& CodeTypes, const HTuple& GenParamName) const;

  // Get parameters that are used by the bar code reader when processing a specific bar code type. 
  HTuple GetBarCodeParamSpecific(const HString& CodeTypes, const HString& GenParamName) const;

  // Get parameters that are used by the bar code reader when processing a specific bar code type. 
  HTuple GetBarCodeParamSpecific(const char* CodeTypes, const char* GenParamName) const;

#ifdef _WIN32
  // Get parameters that are used by the bar code reader when processing a specific bar code type. 
  HTuple GetBarCodeParamSpecific(const wchar_t* CodeTypes, const wchar_t* GenParamName) const;
#endif

  // Get one or several parameters that describe the bar code model.
  HTuple GetBarCodeParam(const HTuple& GenParamName) const;

  // Get one or several parameters that describe the bar code model.
  HTuple GetBarCodeParam(const HString& GenParamName) const;

  // Get one or several parameters that describe the bar code model.
  HTuple GetBarCodeParam(const char* GenParamName) const;

#ifdef _WIN32
  // Get one or several parameters that describe the bar code model.
  HTuple GetBarCodeParam(const wchar_t* GenParamName) const;
#endif

  // Set selected parameters of the bar code model for selected bar code types
  void SetBarCodeParamSpecific(const HTuple& CodeTypes, const HTuple& GenParamName, const HTuple& GenParamValue) const;

  // Set selected parameters of the bar code model for selected bar code types
  void SetBarCodeParamSpecific(const HString& CodeTypes, const HString& GenParamName, const HTuple& GenParamValue) const;

  // Set selected parameters of the bar code model for selected bar code types
  void SetBarCodeParamSpecific(const char* CodeTypes, const char* GenParamName, const HTuple& GenParamValue) const;

#ifdef _WIN32
  // Set selected parameters of the bar code model for selected bar code types
  void SetBarCodeParamSpecific(const wchar_t* CodeTypes, const wchar_t* GenParamName, const HTuple& GenParamValue) const;
#endif

  // Set selected parameters of the bar code model.
  void SetBarCodeParam(const HTuple& GenParamName, const HTuple& GenParamValue) const;

  // Set selected parameters of the bar code model.
  void SetBarCodeParam(const HString& GenParamName, double GenParamValue) const;

  // Set selected parameters of the bar code model.
  void SetBarCodeParam(const char* GenParamName, double GenParamValue) const;

#ifdef _WIN32
  // Set selected parameters of the bar code model.
  void SetBarCodeParam(const wchar_t* GenParamName, double GenParamValue) const;
#endif

  // Delete a bar code model and free the allocated memory
  static void ClearBarCodeModel(const HBarCodeArray& BarCodeHandle);

  // Delete a bar code model and free the allocated memory
  void ClearBarCodeModel() const;

  // Create a model of a bar code reader.
  void CreateBarCodeModel(const HTuple& GenParamName, const HTuple& GenParamValue);

  // Create a model of a bar code reader.
  void CreateBarCodeModel(const HString& GenParamName, double GenParamValue);

  // Create a model of a bar code reader.
  void CreateBarCodeModel(const char* GenParamName, double GenParamValue);

#ifdef _WIN32
  // Create a model of a bar code reader.
  void CreateBarCodeModel(const wchar_t* GenParamName, double GenParamValue);
#endif

};

// forward declarations and types for internal array implementation

template<class T> class HSmartPtr;
template<class T> class HHandleBaseArrayRef;

typedef HHandleBaseArrayRef<HBarCode> HBarCodeArrayRef;
typedef HSmartPtr< HBarCodeArrayRef > HBarCodeArrayPtr;


// Represents multiple tool instances
class LIntExport HBarCodeArray : public HHandleBaseArray
{

public:

  // Create empty array
  HBarCodeArray();

  // Create array from native array of tool instances
  HBarCodeArray(HBarCode* classes, Hlong length);

  // Copy constructor
  HBarCodeArray(const HBarCodeArray &tool_array);

  // Destructor
  virtual ~HBarCodeArray();

  // Assignment operator
  HBarCodeArray &operator=(const HBarCodeArray &tool_array);

  // Clears array and all tool instances
  virtual void Clear();

  // Get array of native tool instances
  const HBarCode* Tools() const;

  // Get number of tools
  virtual Hlong Length() const;

  // Create tool array from tuple of handles
  virtual void SetFromTuple(const HTuple& handles);

  // Get tuple of handles for tool array
  virtual HTuple ConvertToTuple() const;

protected:

// Smart pointer to internal data container
   HBarCodeArrayPtr *mArrayPtr;
};

}

#endif
