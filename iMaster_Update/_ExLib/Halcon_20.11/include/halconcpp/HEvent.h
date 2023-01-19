/***********************************************************
 * File generated by the HALCON-Compiler hcomp version 20.11
 * Usage: Interface to C++
 *
 * Software by: MVTec Software GmbH, www.mvtec.com
 ***********************************************************/


#ifndef HCPP_HEVENT
#define HCPP_HEVENT

namespace HalconCpp
{

// Represents an instance of an event synchronization object.
class LIntExport HEvent : public HHandle
{

public:

  // Create an uninitialized instance
  HEvent():HHandle() {}

  // Copy constructor
  HEvent(const HEvent& source) : HHandle(source) {}

  // Copy constructor
  HEvent(const HHandle& handle);

  // Create HEvent from handle, taking ownership
  explicit HEvent(Hlong handle);

  bool operator==(const HHandle& obj) const
  {
    return HHandleBase::operator==(obj);
  }

  bool operator!=(const HHandle& obj) const
  {
    return HHandleBase::operator!=(obj);
  }

protected:

  // Verify matching semantic type ('event')!
  virtual void AssertType(Hphandle handle) const;

public:



/*****************************************************************************
 * Operator-based class constructors
 *****************************************************************************/

  // create_event: Create an event synchronization object.
  explicit HEvent(const HTuple& AttribName, const HTuple& AttribValue);

  // create_event: Create an event synchronization object.
  explicit HEvent(const HString& AttribName, const HString& AttribValue);

  // create_event: Create an event synchronization object.
  explicit HEvent(const char* AttribName, const char* AttribValue);

#ifdef _WIN32
  // create_event: Create an event synchronization object.
  explicit HEvent(const wchar_t* AttribName, const wchar_t* AttribValue);
#endif




  /***************************************************************************
   * Operators                                                               *
   ***************************************************************************/

  // Clear the event synchronization object.
  void ClearEvent() const;

  // Unlock an event synchronization object. 
  void SignalEvent() const;

  // Lock an event synchronization object only if it is unlocked.
  Hlong TryWaitEvent() const;

  // Lock an event synchronization object.
  void WaitEvent() const;

  // Create an event synchronization object.
  void CreateEvent(const HTuple& AttribName, const HTuple& AttribValue);

  // Create an event synchronization object.
  void CreateEvent(const HString& AttribName, const HString& AttribValue);

  // Create an event synchronization object.
  void CreateEvent(const char* AttribName, const char* AttribValue);

#ifdef _WIN32
  // Create an event synchronization object.
  void CreateEvent(const wchar_t* AttribName, const wchar_t* AttribValue);
#endif

};

// forward declarations and types for internal array implementation

template<class T> class HSmartPtr;
template<class T> class HHandleBaseArrayRef;

typedef HHandleBaseArrayRef<HEvent> HEventArrayRef;
typedef HSmartPtr< HEventArrayRef > HEventArrayPtr;


// Represents multiple tool instances
class LIntExport HEventArray : public HHandleBaseArray
{

public:

  // Create empty array
  HEventArray();

  // Create array from native array of tool instances
  HEventArray(HEvent* classes, Hlong length);

  // Copy constructor
  HEventArray(const HEventArray &tool_array);

  // Destructor
  virtual ~HEventArray();

  // Assignment operator
  HEventArray &operator=(const HEventArray &tool_array);

  // Clears array and all tool instances
  virtual void Clear();

  // Get array of native tool instances
  const HEvent* Tools() const;

  // Get number of tools
  virtual Hlong Length() const;

  // Create tool array from tuple of handles
  virtual void SetFromTuple(const HTuple& handles);

  // Get tuple of handles for tool array
  virtual HTuple ConvertToTuple() const;

protected:

// Smart pointer to internal data container
   HEventArrayPtr *mArrayPtr;
};

}

#endif
