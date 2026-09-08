#pragma once

#include <WString.h>
#include <Print.h>

// ********************************************************************************
// ValueStruct
// ********************************************************************************


class ValueStruct
{
public:

  enum class ValueType : uint8_t {
    Unset = 0,
    String,
    FlashString,
    Float,
    Double,
    Int,
    UInt,
    Bool

  };

  enum class PreferredFormat : uint8_t {
    Default = 0,
    Dec,
    Bin,
    Hex

  };

  enum class CaseFormat : uint8_t {
    KeepCase = 0,
    ToUpper,
    ToLower

  };

  ValueStruct() :
    _isSSO(0),
    _valueType((uint64_t)ValueStruct::ValueType::Unset),
    str_val(nullptr)
  {}

  ~ValueStruct();

  ValueStruct(const ValueStruct& rhs) = delete;
  ValueStruct(ValueStruct&& rhs);

  ValueStruct(const bool& val);

  ValueStruct(int val);
#if defined(ESP32) && !defined(__riscv)
  ValueStruct(int32_t val);
#endif
  ValueStruct(uint32_t val);
#if defined(ESP32) && !defined(__riscv)
  ValueStruct(size_t val);
#endif
  ValueStruct(const uint64_t& val);

  ValueStruct(const int64_t& val);


  ValueStruct(const float& val,
              uint8_t      nrDecimals        = 4,
              bool         trimTrailingZeros = false);

  ValueStruct(const double& val,
              uint8_t       nrDecimals        = 4,
              bool          trimTrailingZeros = false);


  ValueStruct(const char*val);

  ValueStruct(const String& val);

  ValueStruct(String&& val);

  ValueStruct(const __FlashStringHelper *val);

  static ValueStruct     makeHexFormatted(uint64_t val,
                                          uint8_t  minNrDigits);
  static ValueStruct     makeBinFormatted(uint64_t val,
                                          uint8_t  minNrDigits);

  ValueStruct::ValueType getValueType() const
  {
    if (_isSSO) { return ValueStruct::ValueType::String; }
    return static_cast<ValueStruct::ValueType>(_valueType);
  }

  ValueStruct::PreferredFormat getPreferredFormat() const
  {
    return static_cast<ValueStruct::PreferredFormat>(_preferredFormat);
  }

  void                    setPreferredFormat(ValueStruct::PreferredFormat format);

  ValueStruct::CaseFormat getCaseFormat() const {
    return static_cast<ValueStruct::CaseFormat>(_caseFormat);
  }

  void         setCaseFormat(ValueStruct::CaseFormat caseFormat);

  void         setMinNrDigits(uint8_t minNrDigits) { _minNrDigits = (uint64_t)minNrDigits; }

  ValueStruct& operator=(ValueStruct&& rhs);
  ValueStruct& operator=(const ValueStruct& rhs) = delete;

  // We really try to enforce moving the ValueStruct, but when needed a deepcopy is possible
  ValueStruct& deepCopy(const ValueStruct& rhs);

  operator bool() const {
    return getValueType() != ValueStruct::ValueType::Unset;
  }

  // Try to interpret the given string and store the value as compact as possible
  // If the given string is a numerical, try to detect:
  //  - preferred notation (Hex/Dec/Bin)
  //  - number of decimals
  static ValueStruct makeFromString(const __FlashStringHelper *val);
  static ValueStruct makeFromString(const String& val);

  String             toString() const;

  String             toString(ValueType& valueType) const;

  int64_t            toInt() const;

  double             toFloat() const;

  size_t             print(Print& out) const;

  bool               isEmpty() const;

  void               clear();

  bool               isSet() const { return getValueType() != ValueStruct::ValueType::Unset; }

private:

  size_t formatCase(Print  & out,
                    String&& str) const;

  size_t print(Print    & out,
               ValueType& valueType) const;

  union {
    struct {
      uint64_t _isSSO             : 1;
      uint64_t _trimTrailingZeros : 1;
      uint64_t _valueType         : 4;
      uint64_t _preferredFormat   : 2;
      uint64_t _nrDecimals        : 8;
      uint64_t _minNrDigits       : 6;  // For printing ints with leading zeroes
      uint64_t _caseFormat        : 2;
      uint64_t _size              : 16; // Length of string or nr of bits
      uint64_t unused             : 24;

      union {
        void    *str_val;
        float    f_val;
        double   d_val;
        int64_t  i64_val;
        uint64_t u64_val;

      };

    };

    // When _isSSO, the short string will be stored in bytes 1 ... 15
    // The short string will be zero-terminated.
    uint8_t bytes_all[16] = {};

  };


}; // class ValueStruct
