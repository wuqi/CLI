/*
This file is part of ezOptionParser. See MIT-LICENSE.
Copyright (C) 2011,2012,2014 Remik Ziemlinski <first d0t surname att gmail>
CHANGELOG
v0.0.0 20110505 rsz Created.
v0.1.0 20111006 rsz Added validator.
v0.1.1 20111012 rsz Fixed validation of ulonglong.
v0.1.2 20111126 rsz Allow flag names start with alphanumeric (previously, flag had to start with alpha).
v0.1.3 20120108 rsz Created work-around for unique id generation with IDGenerator that avoids retarded c++ translation unit linker errors with single-header static variables. Forced inline on all methods to please retard compiler and avoid multiple def errors.
v0.1.4 20120629 Enforced MIT license on all files.
v0.2.0 20121120 Added parseIndex to OptionGroup.
v0.2.1 20130506 Allow disabling doublespace of OPTIONS usage descriptions.
v0.2.2 20140504 Jose Santiago added compiler warning fixes.
                Bruce Shankle added a crash fix in description printing.
v0.2.3 20150403 Remove ezOptionValidator,set validate function in OptionGroup.
                Support vc6,remove warnings
*/
#ifndef EZ_OPTION_PARSER_H
#define EZ_OPTION_PARSER_H
//disable warnings
#ifdef _MSC_VER
#if _MSC_VER<1400
#pragma warning(disable: 4786)
#else
#define  _CRT_SECURE_NO_WARNINGS
#endif
#endif
#include <stdlib.h>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>
#include <sstream>
#include <cstring>
namespace ez
{
#define DEBUGLINE() printf("%s:%d\n", __FILE__, __LINE__);
  enum EZ_TYPE { EZ_NOTYPE = 0, EZ_BOOL, EZ_INT8, EZ_UINT8, EZ_INT16, EZ_UINT16, EZ_INT32, EZ_UINT32, EZ_INT64, EZ_UINT64, EZ_FLOAT, EZ_DOUBLE, EZ_TEXT };
  static const std::string EZ_TYPE_NAME[] = {"NOTYPE", "bool", "char", "unsigned char", "short", "unsigned short", "int", "unsigned int", "long", "unsigned long", "float", "double", "string"};
  /* ################################################################### */
  template<typename T>
  static T fromString (const std::string* s)
  {
    std::istringstream stream (s->c_str());
    T t;
    stream >> t;
    return t;
  };
  template<typename T>
  static T fromString (const char* s)
  {
    std::istringstream stream (s);
    T t;
    stream >> t;
    return t;
  };
  /* ################################################################### */
  static inline bool isdigit (const std::string& s, int i = 0)
  {
    int n = s.length();

    for (; i < n; ++i)
      switch (s[i]) {
        case '0': case '1': case '2':
        case '3': case '4': case '5':
        case '6': case '7': case '8': case '9': break;

        default: return false;
      }

    return true;
  };
  /* ################################################################### */
  /*
  Compare strings for opts, so short opt flags come before long format flags.
  For example, -d < --dimension < --dmn, and also lower come before upper. The default STL std::string compare doesn't do that.
  */
  static bool CmpOptStringPtr (std::string* s1, std::string* s2)
  {
    int c1, c2;
    const char* s = s1->c_str();

    for (c1 = 0; c1 < (long int)s1->size(); ++c1)
      if (isalnum (s[c1])) { // locale sensitive.
        break;
      }

    s = s2->c_str();

    for (c2 = 0; c2 < (long int)s2->size(); ++c2)
      if (isalnum (s[c2])) {
        break;
      }

    // Test which has more symbols before its name.
    if (c1 > c2) {
      return false;
    } else if (c1 < c2) {
      return true;
    }

    // Both have same number of symbols, so compare first letter.
    char char1 = s1->at (c1);
    char char2 = s2->at (c2);
    char lo1 = tolower (char1);
    char lo2 = tolower (char2);

    if (lo1 != lo2) {
      return lo1 < lo2;
    }

    // Their case doesn't match, so find which is lower.
    char up1 = isupper (char1);
    char up2 = isupper (char2);

    if (up1 && !up2) {
      return false;
    } else if (!up1 && up2) {
      return true;
    }

    return (s1->compare (*s2) < 0);
  };
  /* ################################################################### */
  /*
  Makes a vector of strings from one string,
  splitting at (and excluding) delimiter "token".
  */
  static void SplitDelim (const std::string& s, const char token, std::vector<std::string*>* result)
  {
    std::string::const_iterator i = s.begin();
    std::string::const_iterator j = s.begin();
    const std::string::const_iterator e = s.end();

    while (i != e) {
      while (i != e && *i++ != token);

      std::string* newstr = new std::string (j, i);

      if (newstr->at (newstr->size() - 1) == token) {
        newstr->erase (newstr->size() - 1);
      }

      result->push_back (newstr);
      j = i;
    }
  };
  /* ################################################################### */
  // Variant that uses deep copies and references instead of pointers (less efficient).
  static void SplitDelim (const std::string& s, const char token, std::vector<std::string>& result)
  {
    std::string::const_iterator i = s.begin();
    std::string::const_iterator j = s.begin();
    const std::string::const_iterator e = s.end();

    while (i != e) {
      while (i != e && *i++ != token);

      std::string newstr (j, i);

      if (newstr.at (newstr.size() - 1) == token) {
        newstr.erase (newstr.size() - 1);
      }

      result.push_back (newstr);
      j = i;
    }
  };
  /* ################################################################### */
  // Variant that uses list instead of vector for efficient insertion, etc.
  static void SplitDelim (const std::string& s, const char token, std::list<std::string*>& result)
  {
    std::string::const_iterator i = s.begin();
    std::string::const_iterator j = s.begin();
    const std::string::const_iterator e = s.end();

    while (i != e) {
      while (i != e && *i++ != token);

      std::string* newstr = new std::string (j, i);

      if (newstr->at (newstr->size() - 1) == token) {
        newstr->erase (newstr->size() - 1);
      }

      result.push_back (newstr);
      j = i;
    }
  };
  /* ################################################################### */
  static char** CommandLineToArgvA (char* CmdLine, int* _argc)
  {
    char** argv;
    char*  _argv;
    unsigned long   len;
    unsigned long   argc;
    char   a;
    unsigned long   i, j;
    bool  in_QM;
    bool  in_TEXT;
    bool  in_SPACE;
    len = strlen (CmdLine);
    i = ((len + 2) / 2) * sizeof (void*) + sizeof (void*);
    argv = (char**)malloc (i + (len + 2) * sizeof (char));
    _argv = (char*) (((unsigned char*)argv) + i);
    argc = 0;
    argv[argc] = _argv;
    in_QM = false;
    in_TEXT = false;
    in_SPACE = true;
    i = 0;
    j = 0;

    while ((a = CmdLine[i])) {
      if (in_QM) {
        if ((a == '\"') ||
            (a == '\'')) { // rsz. Added single quote.
          in_QM = false;
        } else {
          _argv[j] = a;
          j++;
        }
      } else {
        switch (a) {
          case '\"':
          case '\'': // rsz. Added single quote.
            in_QM = true;
            in_TEXT = true;

            if (in_SPACE) {
              argv[argc] = _argv + j;
              argc++;
            }

            in_SPACE = false;
            break;

          case ' ':
          case '\t':
          case '\n':
          case '\r':
            if (in_TEXT) {
              _argv[j] = '\0';
              j++;
            }

            in_TEXT = false;
            in_SPACE = true;
            break;

          default:
            in_TEXT = true;

            if (in_SPACE) {
              argv[argc] = _argv + j;
              argc++;
            }

            _argv[j] = a;
            j++;
            in_SPACE = false;
            break;
        }
      }

      i++;
    }

    _argv[j] = '\0';
    argv[argc] = NULL;
    (*_argc) = argc;
    return argv;
  };
  /* ################################################################### */
  class OptionGroup
  {
  public:
    OptionGroup() : delim (0), expectArgs (0), isRequired (false), isSet (false) , needValidate (false), typeenum (EZ_NOTYPE) { }
    ~OptionGroup()
    {
      int i;

      for (i = 0; i < (long int)flags.size(); ++i) {
        delete flags[i];
      }

      flags.clear();
      parseIndex.clear();
      clearArgs();
    };
    inline void clearArgs();
    //template <typename T>
    //inline void get (T&);
    // template <typename T>
    // inline void getVector (std::vector<T>&);
    // template <typename T>
    // inline void getMultiVector (std::vector< std::vector<T> >&);
    inline bool validate (std::vector<std::string>&);
    // template<typename T>
    // inline bool validate (T , std::vector<std::string>&);
    // defaults value regardless of being set by user.
    std::string defaults;
    // If expects arguments, this will delimit arg list.
    char delim;
    // If not 0, then number of delimited args. -1 for arbitrary number.
    int expectArgs;
    // Descriptive help message shown in usage instructions for option.
    std::string help;
    // 0 or 1.
    bool isRequired;
    // A list of flags that denote this option, i.e. -d, --dimension.
    std::vector< std::string* > flags;
    // If was set (or found).
    bool isSet;
    //need Validate
    bool needValidate;
    // Lists of arguments, per flag instance, after splitting by delimiter.
    std::vector< std::vector< std::string* > * > args;
    // Index where each group was parsed from input stream to track order.
    std::vector<int> parseIndex;
    //set valid values
    std::vector<std::string> validvalues;
    //set maxvalue
    std::string maxvalue;
    //set minvalue
    std::string minvalue;
    //typename
    EZ_TYPE typeenum;
    /* ################################################################### */
    template <typename T>
    inline void get (T& out)
    {
      std::stringstream ss;

      if (!isSet) {
        if (defaults.empty()) {
          ss << "";
          ss >> out;
        } else {
          ss << defaults;
          ss >> out;
        }
      } else {
        if (args.empty() || args[0]->empty()) {
          ss << "";
          ss >> out;
        } else {
          ss << *args[0]->at (0);
          ss >> out;
        }
      }
    };
    /* ################################################################### */
    template <typename T>
    inline void getVector (std::vector<T>& out)
    {
      std::stringstream ss;

      if (!isSet) {
        if (!defaults.empty()) {
          std::vector< std::string > strings;
          SplitDelim (defaults, delim, strings);

          //StringsToInts(strings, out);
          for (int i = 0; i < (long int)strings.size(); ++i) {
            ss << strings[i];
            T temp;
            ss >> temp;
            out.push_back (temp);
            ss.clear();
          }
        }
      } else {
        if (! (args.empty() || args[0]->empty()))

          //StringsToInts(args[0], &out);
          for (int i = 0; i < (long int)args[0]->size(); ++i) {
            ss << *args[0]->at (i);
            T temp;
            ss >> temp;
            out.push_back (temp);
            ss.clear();
          }
      }
    };
    /* ################################################################### */
    template <typename T>
    inline void getMultiVector (std::vector< std::vector<T> >& out)
    {
      std::stringstream ss;

      if (!isSet) {
        if (!defaults.empty()) {
          std::vector< std::string > strings;
          SplitDelim (defaults, delim, strings);

          if (out.size() < 1) {
            out.resize (1);
          }

          //StringsToInts(strings, out[0]);
          for (int i = 0; i < (long int)strings.size(); ++i) {
            ss << strings[i];
            T temp;
            ss >> temp;
            out[0].push_back (temp);
            ss.clear();
          }
        }
      } else {
        if (!args.empty()) {
          int n = args.size();

          if ((long int)out.size() < n) {
            out.resize (n);
          }

          for (int i = 0; i < n; ++i) {
            //StringsToInts(args[i], &out[i]);
            for (int j = 0; j < (long int)args[i]->size(); ++j) {
              ss << *args[i]->at (j);
              T temp;
              ss >> temp;
              out[i].push_back (temp);
              ss.clear();
            }
          }
        }
      }
    };
    /* ################################################################### */
    template<typename T>
    inline bool validate (T example, std::vector<std::string>& badOptions)
    {
      std::stringstream ss;

      if (validvalues.size() == 0) {
        //check range
        T mint = example, maxt = example;

        if (!minvalue.empty()) {
          ss << minvalue;
          ss >> mint;
          ss.clear();
        } else {
          mint = std::numeric_limits<T>::min();
        }

        if (!maxvalue.empty()) {
          ss << maxvalue;
          ss >> maxt;
          ss.clear();
        } else {
          maxt = std::numeric_limits<T>::max();
        }

        double min, max;
        ss << mint;
        ss >> min;
        ss.clear();
        ss.str ("");
        ss << maxt;
        ss >> max;
        ss.clear();
        ss.str ("");

        //iterates all arguments
        for (int i = 0; i < (int)args.size(); ++i) {
          for (int j = 0; j < (int)args[i]->size(); ++j) {
            ss << *args[i]->at (j);
            double value;
            ss >> value;
            ss.clear();
            ss.str ("");

            if (value > max || value < min) {
              badOptions.push_back (*args[i]->at (j));
            }
          }
        }
      } else {
        //check arguments whether in the valid value list
        for (int i = 0; i < (int)args.size(); ++i) {
          for (int j = 0; j < (int)args[i]->size(); ++j) {
            std::string arg = *args[i]->at (j);

            if (std::find (validvalues.begin(), validvalues.end(), arg) == validvalues.end()) {
              badOptions.push_back (*args[i]->at (j));
            }
          }
        }
      }

      return badOptions.empty();
    };
  };
  /* ################################################################### */
  void OptionGroup::clearArgs()
  {
    int i, j;

    for (i = 0; i < (long int)args.size(); ++i) {
      for (j = 0; j < (long int)args[i]->size(); ++j) {
        delete args[i]->at (j);
      }

      delete args[i];
    }

    args.clear();
    isSet = false;
  };
  /* ################################################################### */
  bool OptionGroup::validate (std::vector<std::string>& badOptions)
  {
    if (needValidate) {
      //need Validate
      //check type
      switch (typeenum) {
        case EZ_BOOL: {
          bool example = true;
          return validate (example, badOptions);
        }

        case EZ_INT8: {
          char example = 0;
          return validate (example, badOptions);
        }

        case EZ_UINT8: {
          unsigned char example = 0;
          return validate (example, badOptions);
        }

        case EZ_INT16: {
          short example = 0;
          return validate (example, badOptions);
        }

        case EZ_UINT16: {
          unsigned short example = 0;
          return validate (example, badOptions);
        }

        case EZ_INT32: {
          int example = 0;
          return validate (example, badOptions);
        }

        case EZ_UINT32: {
          unsigned int example = 0;
          return validate (example, badOptions);
        }

        case EZ_INT64: {
          long example = 0;
          return validate (example, badOptions);
        }

        case EZ_UINT64: {
          unsigned long example = 0;
          return validate (example, badOptions);
        }

        case EZ_FLOAT: {
          float example = 0;
          return validate (example, badOptions);
        }

        case EZ_DOUBLE: {
          double example = 0;
          return validate (example, badOptions);
        }

        case EZ_TEXT: {
          std::string example = "";
          return validate (example, badOptions);
        }

        case EZ_NOTYPE:
        default:
          break;
      }
    }

    return badOptions.empty();
  };
  /* ################################################################### */
  class ezOptionParser
  {
  public:
    ezOptionParser()
    {
      add (
        "", // 默认值,可以设置为空.
        0, // 是否为必须?是则为1
        0, // 指定参数个数，-1用于传入列表,指定-1时最少需要1个参数.
        "Print this usage message.", // Usage里显示的信息.
        "-h",     // Flag token,最多可以指定四个.
        "-help", // Flag token.
        "--help", // Flag token.
        "--usage" // Flag token.
      );
    };
    // How to layout usage descriptions with the option flags.
    enum Layout { ALIGN, INTERLEAVE, STAGGER };
    inline ~ezOptionParser();
    inline void add (const char* defaults, bool required, int expectArgs, const char* help, const char* flag1, EZ_TYPE typeEnum = EZ_NOTYPE, const std::string& minValue = std::string(), const std::string& maxValue = std::string(), const std::vector<std::string>& validList = std::vector<std::string>());
    inline void add (const char* defaults, bool required, int expectArgs, const char* help, const char* flag1, const char* flag2, EZ_TYPE typeEnum = EZ_NOTYPE, const std::string& minValue = std::string(), const std::string& maxValue = std::string(), const std::vector<std::string>& validList = std::vector<std::string>());
    inline void add (const char* defaults, bool required, int expectArgs, const char* help, const char* flag1, const char* flag2, const char* flag3, EZ_TYPE typeEnum = EZ_NOTYPE, const std::string& minValue = std::string(), const std::string& maxValue = std::string(), const std::vector<std::string>& validList = std::vector<std::string>());
    inline void add (const char* defaults, bool required, int expectArgs, const char* help, const char* flag1, const char* flag2, const char* flag3, const char* flag4, EZ_TYPE typeEnum = EZ_NOTYPE, const std::string& minValue = std::string(), const std::string& maxValue = std::string(), const std::vector<std::string>& validList = std::vector<std::string>());
    inline void xorAdd (std::string a, std::string b);
    inline void xorAdd (std::vector<std::string>&);
    inline bool exportFile (const char* filename, bool all = false);
    inline OptionGroup* get (const char* name);
    inline void getUsage (std::string& usage, int width = 80, Layout layout = INTERLEAVE);
    inline void getUsageDescriptions (std::string& usage, int width = 80, Layout layout = STAGGER);
    inline bool gotExpected (std::vector<std::string>& badOptions);
    inline bool gotRequired (std::vector<std::string>& badOptions);
    inline bool gotValid (std::vector<std::string>& badOptions, std::vector<std::string>& badArgs);
    inline bool importFile (const char* filename, char comment = '#');
    inline int isSet (const char* name);
    inline int isSet (std::string& name);
    inline void parse (int argc, const char* argv[]);
    inline void prettyPrint (std::string& out);
    inline void reset();
    inline void resetArgs();
    inline bool checkValid (std::string& out);
    // Insert extra empty line betwee each option's usage description.
    char doublespace;
    // General description in human language on what the user's tool does.
    // It's the first section to get printed in the full usage message.
    std::string overview;
    // A synopsis of command and options usage to show expected order of input arguments.
    // It's the second section to get printed in the full usage message.
    std::string syntax;
    // Example (third) section in usage message.
    std::string example;
    // Final section printed in usage message. For contact, copyrights, version info.
    std::string footer;
    // Map from an option to an Id of its parent group.
    std::map< std::string, int > optionGroupIds;
    // Unordered collection of the option groups.
    std::vector< OptionGroup* > groups;
    // xor group
    std::vector< OptionGroup*> xorGroups;
    // Store unexpected args in input.
    std::vector< std::string > unknownArgs;
    // List of args that occur left-most before first option flag.
    std::vector< std::string* > firstArgs;
    // List of args that occur after last right-most option flag and its args.
    std::vector< std::string* > lastArgs;
  private:
    //progName
    std::string _progName;
  };
  /* ################################################################### */
  ezOptionParser::~ezOptionParser()
  {
    reset();
  }
  /* ################################################################### */
  void ezOptionParser::reset()
  {
    this->doublespace = 1;
    int i;

    for (i = 0; i < (long int)groups.size(); ++i) {
      delete groups[i];
    }

    groups.clear();
    unknownArgs.clear();

    for (i = 0; i < (long int)firstArgs.size(); ++i) {
      delete firstArgs[i];
    }

    firstArgs.clear();

    for (i = 0; i < (long int)lastArgs.size(); ++i) {
      delete lastArgs[i];
    }

    lastArgs.clear();
    optionGroupIds.clear();
  };
  /* ################################################################### */
  void ezOptionParser::resetArgs()
  {
    int i;

    for (i = 0; i < (long int)groups.size(); ++i) {
      groups[i]->clearArgs();
    }

    unknownArgs.clear();

    for (i = 0; i < (long int)firstArgs.size(); ++i) {
      delete firstArgs[i];
    }

    firstArgs.clear();

    for (i = 0; i < (long int)lastArgs.size(); ++i) {
      delete lastArgs[i];
    }

    lastArgs.clear();
  };
  /* ################################################################### */
  void ezOptionParser::add (const char* defaults, bool required, int expectArgs, const char* help, const char* flag1, EZ_TYPE typeEnum, const std::string& minValue, const std::string& maxValue, const std::vector<std::string>& validList)
  {
    int id = this->groups.size();
    OptionGroup* g = new OptionGroup;
    g->defaults = defaults;
    g->isRequired = required;
    g->expectArgs = expectArgs;
    g->delim = ',';
    g->isSet = 0;

    if (typeEnum != EZ_NOTYPE) {
      g->help += "[";
      g->help += EZ_TYPE_NAME[typeEnum];
      g->help += "] ";
    }

    g->help += help;
    std::string* f1 = new std::string (flag1);
    g->flags.push_back (f1);
    g->typeenum = typeEnum;
    g->minvalue = minValue;
    g->maxvalue = maxValue;
    g->validvalues = validList;

    if (typeEnum == EZ_TEXT && validList.size() > 0) {
      g->needValidate = true;
    } else if (typeEnum != EZ_TEXT && typeEnum != EZ_NOTYPE) {
      g->needValidate = true;
    }

    this->optionGroupIds[flag1] = id;
    this->groups.push_back (g);
  };
  /* ################################################################### */
  void ezOptionParser::add (const char* defaults, bool required, int expectArgs, const char* help, const char* flag1, const char* flag2, EZ_TYPE typeEnum, const std::string& minValue, const std::string& maxValue, const std::vector<std::string>& validList)
  {
    int id = this->groups.size();
    OptionGroup* g = new OptionGroup;
    g->defaults = defaults;
    g->isRequired = required;
    g->expectArgs = expectArgs;
    g->delim = ',';
    g->isSet = 0;

    if (typeEnum != EZ_NOTYPE) {
      g->help += "[";
      g->help += EZ_TYPE_NAME[typeEnum];
      g->help += "] ";
    }

    g->help += help;
    std::string* f1 = new std::string (flag1);
    g->flags.push_back (f1);
    std::string* f2 = new std::string (flag2);
    g->flags.push_back (f2);
    g->typeenum = typeEnum;
    g->minvalue = minValue;
    g->maxvalue = maxValue;
    g->validvalues = validList;

    if (typeEnum == EZ_TEXT && validList.size() > 0) {
      g->needValidate = true;
    } else if (typeEnum != EZ_TEXT && typeEnum != EZ_NOTYPE) {
      g->needValidate = true;
    }

    this->optionGroupIds[flag1] = id;
    this->optionGroupIds[flag2] = id;
    this->groups.push_back (g);
  };
  /* ################################################################### */
  void ezOptionParser::add (const char* defaults, bool required, int expectArgs, const char* help, const char* flag1, const char* flag2, const char* flag3, EZ_TYPE typeEnum, const std::string& minValue, const std::string& maxValue, const std::vector<std::string>& validList)
  {
    int id = this->groups.size();
    OptionGroup* g = new OptionGroup;
    g->defaults = defaults;
    g->isRequired = required;
    g->expectArgs = expectArgs;
    g->delim = ',';
    g->isSet = 0;

    if (typeEnum != EZ_NOTYPE) {
      g->help += "[";
      g->help += EZ_TYPE_NAME[typeEnum];
      g->help += "] ";
    }

    g->help += help;
    std::string* f1 = new std::string (flag1);
    g->flags.push_back (f1);
    std::string* f2 = new std::string (flag2);
    g->flags.push_back (f2);
    std::string* f3 = new std::string (flag3);
    g->flags.push_back (f3);
    g->typeenum = typeEnum;
    g->minvalue = minValue;
    g->maxvalue = maxValue;
    g->validvalues = validList;

    if (typeEnum == EZ_TEXT && validList.size() > 0) {
      g->needValidate = true;
    } else if (typeEnum != EZ_TEXT && typeEnum != EZ_NOTYPE) {
      g->needValidate = true;
    }

    this->optionGroupIds[flag1] = id;
    this->optionGroupIds[flag2] = id;
    this->optionGroupIds[flag3] = id;
    this->groups.push_back (g);
  };
  /* ################################################################### */
  void ezOptionParser::add (const char* defaults, bool required, int expectArgs, const char* help, const char* flag1, const char* flag2, const char* flag3, const char* flag4, EZ_TYPE typeEnum, const std::string& minValue, const std::string& maxValue, const std::vector<std::string>& validList)
  {
    int id = this->groups.size();
    OptionGroup* g = new OptionGroup;
    g->defaults = defaults;
    g->isRequired = required;
    g->expectArgs = expectArgs;
    g->delim = ',';
    g->isSet = 0;

    if (typeEnum != EZ_NOTYPE) {
      g->help += "[";
      g->help += EZ_TYPE_NAME[typeEnum];
      g->help += "] ";
    }

    g->help += help;
    std::string* f1 = new std::string (flag1);
    g->flags.push_back (f1);
    std::string* f2 = new std::string (flag2);
    g->flags.push_back (f2);
    std::string* f3 = new std::string (flag3);
    g->flags.push_back (f3);
    std::string* f4 = new std::string (flag4);
    g->flags.push_back (f4);
    g->typeenum = typeEnum;
    g->minvalue = minValue;
    g->maxvalue = maxValue;
    g->validvalues = validList;

    if (typeEnum == EZ_TEXT && validList.size() > 0) {
      g->needValidate = true;
    } else if (typeEnum != EZ_TEXT && typeEnum != EZ_NOTYPE) {
      g->needValidate = true;
    }

    this->optionGroupIds[flag1] = id;
    this->optionGroupIds[flag2] = id;
    this->optionGroupIds[flag3] = id;
    this->optionGroupIds[flag4] = id;
    this->groups.push_back (g);
  };
  /* ################################################################### */
  bool ezOptionParser::exportFile (const char* filename, bool all)
  {
    int i;
    std::string out;
    bool quote;

    // Export the first args, except the program name, so start from 1.
    for (i = 1; i < (long int)firstArgs.size(); ++i) {
      quote = ((firstArgs[i]->find_first_of (" \t") != std::string::npos) && (firstArgs[i]->find_first_of ("\'\"") == std::string::npos));

      if (quote) {
        out.append ("\"");
      }

      out.append (*firstArgs[i]);

      if (quote) {
        out.append ("\"");
      }

      out.append (" ");
    }

    if (firstArgs.size() > 1) {
      out.append ("\n");
    }

    std::vector<std::string* > stringPtrs (groups.size());
    int m;
    int n = groups.size();

    for (i = 0; i < n; ++i) {
      stringPtrs[i] = groups[i]->flags[0];
    }

    OptionGroup* g;
    // Sort first flag of each group with other groups.
    std::sort (stringPtrs.begin(), stringPtrs.end(), CmpOptStringPtr);

    for (i = 0; i < n; ++i) {
      g = get (stringPtrs[i]->c_str());

      if (g->isSet || all) {
        if (!g->isSet || g->args.empty()) {
          if (!g->defaults.empty()) {
            out.append (*stringPtrs[i]);
            out.append (" ");
            quote = ((g->defaults.find_first_of (" \t") != std::string::npos) && (g->defaults.find_first_of ("\'\"") == std::string::npos));

            if (quote) {
              out.append ("\"");
            }

            out.append (g->defaults);

            if (quote) {
              out.append ("\"");
            }

            out.append ("\n");
          }
        } else {
          int n = g->args.size();

          for (int j = 0; j < n; ++j) {
            out.append (*stringPtrs[i]);
            out.append (" ");
            m = g->args[j]->size();

            for (int k = 0; k < m; ++k) {
              quote = ((*g->args[j]->at (k)).find_first_of (" \t") != std::string::npos);

              if (quote) {
                out.append ("\"");
              }

              out.append (*g->args[j]->at (k));

              if (quote) {
                out.append ("\"");
              }

              if ((g->delim) && ((k + 1) != m)) {
                out.append (1, g->delim);
              }
            }

            out.append ("\n");
          }
        }
      }
    }

    // Export the last args.
    for (i = 0; i < (long int)lastArgs.size(); ++i) {
      quote = (lastArgs[i]->find_first_of (" \t") != std::string::npos);

      if (quote) {
        out.append ("\"");
      }

      out.append (*lastArgs[i]);

      if (quote) {
        out.append ("\"");
      }

      out.append (" ");
    }

    std::ofstream file (filename);

    if (!file.is_open()) {
      return false;
    }

    file << out;
    file.close();
    return true;
  };
  /* ################################################################### */
  // Does not overwrite current options.
  // Returns true if file was read successfully.
  // So if this is used before parsing CLI, then option values will reflect
  // this file, but if used after parsing CLI, then values will contain
  // both CLI values and file's values.
  //
  // Comment lines are allowed if prefixed with #.
  // Strings should be quoted as usual.
  bool ezOptionParser::importFile (const char* filename, char comment)
  {
    std::ifstream file (filename, std::ios::in | std::ios::ate);

    if (!file.is_open()) {
      return false;
    }

    // Read entire file contents.
    std::ifstream::pos_type size = file.tellg();
    char* memblock = new char[ (int)size + 1]; // Add one for end of string.
    file.seekg (0, std::ios::beg);
    file.read (memblock, size);
    memblock[ (int)size] = '\0';
    file.close();
    // Find comment lines.
    std::list<std::string*> lines;
    std::string memblockstring (memblock);
    delete[] memblock;
    SplitDelim (memblockstring, '\n', lines);
    int i, j, n;
    std::list<std::string*>::iterator iter;
    std::vector<int> sq, dq; // Single and double quote indices.
    std::vector<int>::iterator lo; // For searching quote indices.
    size_t pos;
    const char* str;
    std::string* line;

    // Find all single and double quotes to correctly handle comment tokens.
    for (iter = lines.begin(); iter != lines.end(); ++iter) {
      line = *iter;
      str = line->c_str();
      n = line->size();
      sq.clear();
      dq.clear();

      if (n) {
        // If first char is comment, then erase line and continue.
        pos = line->find_first_not_of (" \t\r");

        if ((pos == std::string::npos) || (line->at (pos) == comment)) {
          line->erase();
          continue;
        } else {
          // Erase whitespace prefix.
          line->erase (0, pos);
          n = line->size();
        }

        if (line->at (0) == '"') {
          dq.push_back (0);
        }

        if (line->at (0) == '\'') {
          sq.push_back (0);
        }
      } else { // Empty line.
        continue;
      }

      for (i = 1; i < n; ++i) {
        if ((str[i] == '"') && (str[i - 1] != '\\')) {
          dq.push_back (i);
        } else if ((str[i] == '\'') && (str[i - 1] != '\\')) {
          sq.push_back (i);
        }
      }

      // Scan for comments, and when found, check bounds of quotes.
      // Start with second char because already checked first char.
      for (i = 1; i < n; ++i) {
        if ((line->at (i) == comment) && (line->at (i - 1) != '\\')) {
          // If within open/close quote pair, then not real comment.
          if (sq.size()) {
            lo = std::lower_bound (sq.begin(), sq.end(), i);
            // All start of strings will be even indices, closing quotes is odd indices.
            j = (int) (lo - sq.begin());

            if ((j % 2) == 0) {  // Even implies comment char not in quote pair.
              // Erase from comment char to end of line.
              line->erase (i);
              break;
            }
          }  else if (dq.size()) {
            // Repeat tests for double quotes.
            lo = std::lower_bound (dq.begin(), dq.end(), i);
            j = (int) (lo - dq.begin());

            if ((j % 2) == 0) {
              line->erase (i);
              break;
            }
          } else {
            // Not in quotes.
            line->erase (i);
            break;
          }
        }
      }
    }

    std::string cmd;

    // Convert list to string without newlines to simulate commandline.
    for (iter = lines.begin(); iter != lines.end(); ++iter) {
      if (! (*iter)->empty()) {
        cmd.append (**iter);
        cmd.append (" ");
      }
    }

    // Now parse as if from command line.
    int argc = 0;
    char** argv = CommandLineToArgvA ((char*)cmd.c_str(), &argc);
    // Parse.
    parse (argc, (const char**)argv);

    if (argv) {
      free (argv);
    }

    for (iter = lines.begin(); iter != lines.end(); ++iter) {
      delete *iter;
    }

    return true;
  };
  /* ################################################################### */
  int ezOptionParser::isSet (const char* name)
  {
    std::string sname (name);

    if (this->optionGroupIds.count (sname)) {
      return this->groups[this->optionGroupIds[sname]]->isSet;
    }

    return 0;
  };
  /* ################################################################### */
  int ezOptionParser::isSet (std::string& name)
  {
    if (this->optionGroupIds.count (name)) {
      return this->groups[this->optionGroupIds[name]]->isSet;
    }

    return 0;
  };
  /* ################################################################### */
  OptionGroup* ezOptionParser::get (const char* name)
  {
    if (optionGroupIds.count (name)) {
      return groups[optionGroupIds[name]];
    }

    return 0;
  };
  /* ################################################################### */
  void ezOptionParser::getUsage (std::string& usage, int width, Layout layout)
  {
    std::stringstream ss;
    usage.append ("NAME: \n\n    ");
    usage.append (overview);
    usage.append ("\n\n");
    usage.append ("USAGE: \n\n    ");
    bool xorset = false;

    if (syntax.empty()) {
      syntax += _progName;
      syntax += " ";

      for (unsigned int i = 0; i < groups.size(); i++) {
        OptionGroup* g = groups[i];
        std::string opt = *g->flags[0];

        if ((std::find (xorGroups.begin(), xorGroups.end(), g) == xorGroups.end())) {
          //no xor
          if (!g->isRequired) {
            opt = "[" + opt;
          }

          if (g->expectArgs > 1) {
            opt += " ";

            for (int j = 0; j < g->expectArgs; ++j) {
              std::string jstr;
              ss << j + 1;
              ss >> jstr;
              ss.clear();
              opt += "Arg";
              opt += jstr;
              opt += ",";
            }

            opt.erase (opt.size() - 1); //remove last comma
          } else if (g->expectArgs == 1) {
            opt += " ";

            if (g->validvalues.size() == 0) {
              opt += "Arg";
            } else {
              for (int k = 0; k < (int)g->validvalues.size(); ++k) {
                opt += g->validvalues[k];
                opt += "|";
              }

              opt.erase (opt.size() - 1);
            }
          } else if (g->expectArgs == -1) {
            opt += " Arg1,[ArgN] ";
          }

          if (!g->isRequired) {
            opt += "] ";
          }
        } else if (!xorset) {
          opt = "";

          if (!g->isRequired) {
            opt = "[";
          }

          xorset = true;

          for (int k = 0; k < (int)xorGroups.size(); ++k) {
            OptionGroup* x = xorGroups[k];
            std::string name = *x->flags[0];
            opt += name;

            if (x->expectArgs > 1) {
              opt += " ";

              for (int j = 0; j < x->expectArgs; ++j) {
                std::string jstr;
                ss << j + 1;
                ss >> jstr;
                ss.clear();
                opt += "Arg";
                opt += jstr;
                opt += ",";
              }

              opt.erase (opt.size() - 1); //remove last comma
            } else if (x->expectArgs == 1) {
              opt += " ";

              if (x->validvalues.size() == 0) {
                opt += "Arg";
              } else {
                for (int k = 0; k < (int)x->validvalues.size(); ++k) {
                  opt += x->validvalues[k];
                  opt += "|";
                }

                opt.erase (opt.size() - 1);
              }
            } else if (x->expectArgs == -1) {
              opt += " Arg1,[ArgN] ";
            }

            opt += "|";
          }

          opt.erase (opt.size() - 1); //rempve last ,

          if (!g->isRequired) {
            opt += "] ";
          }

          opt += " ";
        } else {
          continue;
        }

        syntax += opt;
      }
    }

    usage.append (syntax);
    usage.append ("\n\nOPTIONS:\n\n");
    getUsageDescriptions (usage, width, layout);

    if (!example.empty()) {
      usage.append ("EXAMPLES:\n\n    ");
      usage.append (example);
    }

    if (!footer.empty()) {
      usage.append (footer);
    }
  };
  /* ################################################################### */
  // Creates 2 column formatted help descriptions for each option flag.
  void ezOptionParser::getUsageDescriptions (std::string& usage, int width, Layout layout)
  {
    // Sort each flag list amongst each group.
    int i;
    // Store index of flag groups before sort for easy lookup later.
    std::map<std::string*, int> stringPtrToIndexMap;
    std::vector<std::string* > stringPtrs (groups.size());

    for (i = 0; i < (long int)groups.size(); ++i) {
      std::sort (groups[i]->flags.begin(), groups[i]->flags.end(), CmpOptStringPtr);
      stringPtrToIndexMap[groups[i]->flags[0]] = i;
      stringPtrs[i] = groups[i]->flags[0];
    }

    size_t j, k;
    std::string opts;
    std::vector<std::string> sortedOpts;
    // Sort first flag of each group with other groups.
    std::sort (stringPtrs.begin(), stringPtrs.end(), CmpOptStringPtr);

    for (i = 0; i < (long int)groups.size(); ++i) {
      //printf("DEBUG:%d: %d %d %s\n", __LINE__, i, stringPtrToIndexMap[stringPtrs[i]], stringPtrs[i]->c_str());
      k = stringPtrToIndexMap[stringPtrs[i]];
      opts = "";
      opts.append ("    ");

      for (j = 0; j < groups[k]->flags.size() - 1; ++j) {
        opts.append (*groups[k]->flags[j]);
        opts.append (", ");

        if ((long int)opts.size() > width) {
          opts.append ("\n");
        }
      }

      // The last flag. No need to append comma anymore.
      opts.append (*groups[k]->flags[j]);
      std::stringstream ss;

      if (groups[k]->expectArgs > 1) {
        opts += " ";

        for (int j = 0; j < groups[k]->expectArgs; ++j) {
          std::string jstr;
          ss << j + 1;
          ss >> jstr;
          ss.clear();
          opts += "Arg";
          opts += jstr;
          opts += ",";
        }

        opts.erase (opts.size() - 1); //remove last comma
      } else if (groups[k]->expectArgs == 1) {
        opts += " ";

        if (groups[k]->validvalues.size() == 0) {
          opts += "Arg";
        } else {
          for (int k = 0; k < (int)groups[k]->validvalues.size(); ++k) {
            opts += groups[k]->validvalues[k];
            opts += "|";
          }

          opts.erase (opts.size() - 1);
        }
      } else if (groups[k]->expectArgs == -1) {
        opts += " Arg1,[ArgN] ";
      }

      opts.append (":");
      sortedOpts.push_back (opts);
    }

    // Each option group will use this to build multiline help description.
    std::list<std::string*> desc;
    // Number of whitespaces from start of line to description (interleave layout) or
    // gap between flag names and description (align, stagger layouts).
    int gutter = 3;
    // Find longest opt flag string to set column start for help usage descriptions.
    int maxlen = 0;

    if (layout == ALIGN) {
      for (i = 0; i < (long int)groups.size(); ++i) {
        if (maxlen < (long int)sortedOpts[i].size()) {
          maxlen = sortedOpts[i].size();
        }
      }
    }

    // The amount of space remaining on a line for help text after flags.
    int helpwidth;
    std::list<std::string*>::iterator cIter, insertionIter;
    size_t pos;

    for (i = 0; i < (long int)groups.size(); ++i) {
      k = stringPtrToIndexMap[stringPtrs[i]];

      if (layout == STAGGER) {
        maxlen = sortedOpts[i].size();
      }

      int pad = gutter + maxlen;
      helpwidth = width - pad;
      // All the following split-fu could be optimized by just using substring (offset, length) tuples, but just to get it done, we'll do some not-too expensive string copying.
      SplitDelim (groups[k]->help, '\n', desc);

      // Split lines longer than allowable help width.
      for (insertionIter = desc.begin(), cIter = insertionIter++;
           cIter != desc.end(), insertionIter != desc.end();
           cIter = insertionIter++) {
        if ((long int) ((*cIter)->size()) > helpwidth) {
          // Get pointer to next string to insert new strings before it.
          std::string* rem = *cIter;
          // Remove this line and add back in pieces.
          cIter = desc.erase (cIter);

          // Loop until remaining string is short enough.
          while ((long int)rem->size() > helpwidth) {
            // Find whitespace to split before helpwidth.
            if (rem->at (helpwidth) == ' ') {
              // If word ends exactly at helpwidth, then split after it.
              pos = helpwidth;
            } else {
              // Otherwise, split occurs midword, so find whitespace before this word.
              pos = rem->rfind (" ", helpwidth);
            }

            // Insert split string.
            desc.insert (insertionIter, new std::string (*rem, 0, pos));
            // Now skip any whitespace to start new line.
            pos = rem->find_first_not_of (' ', pos);
            rem->erase (0, pos);
          }

          if (rem->size()) {
            desc.insert (insertionIter, rem);
          } else {
            delete rem;
          }
        }
      }

      usage.append (sortedOpts[i]);

      if (layout != INTERLEAVE)
        // Add whitespace between option names and description.
      {
        usage.append (pad - sortedOpts[i].size(), ' ');
      } else {
        usage.append ("\n    ");
        usage.append (gutter, ' ');
      }

      if (desc.size() > 0) { // Crash fix by Bruce Shankle.
        // First line already padded above (before calling SplitDelim) after option flag names.
        cIter = desc.begin();
        usage.append (**cIter);
        usage.append ("\n");

        // Now inject the pad for each line.
        for (++cIter; cIter != desc.end(); ++cIter) {
          usage.append (pad, ' ');
          usage.append (**cIter);
          usage.append ("\n");
        }

        if (this->doublespace) {
          usage.append ("\n");
        }

        for (cIter = desc.begin(); cIter != desc.end(); ++cIter) {
          delete *cIter;
        }

        desc.clear();
      }
    }
  };
  /* ################################################################### */
  bool ezOptionParser::gotExpected (std::vector<std::string>& badOptions)
  {
    int i, j;

    for (i = 0; i < (long int)groups.size(); ++i) {
      OptionGroup* g = groups[i];

      // If was set, ensure number of args is correct.
      if (g->isSet) {
        if ((g->expectArgs != 0) && g->args.empty()) {
          badOptions.push_back (*g->flags[0]);
          continue;
        }

        for (j = 0; j < (long int)g->args.size(); ++j) {
          if ((g->expectArgs != -1) && (g->expectArgs != (long int)g->args[j]->size())) {
            badOptions.push_back (*g->flags[0]);
          } else if ((g->expectArgs == -1) && (g->args[j]->size() == 0)) {
            //at least 1
            badOptions.push_back (*g->flags[0]);
          }
        }
      }
    }

    return badOptions.empty();
  };
  /* ################################################################### */
  bool ezOptionParser::gotRequired (std::vector<std::string>& badOptions)
  {
    int i;

    for (i = 0; i < (long int)groups.size(); ++i) {
      OptionGroup* g = groups[i];

      // Simple case when required but user never set it.
      if (g->isRequired && (!g->isSet)) {
        badOptions.push_back (*g->flags[0]);
        continue;
      }
    }

    return badOptions.empty();
  };
  /* ################################################################### */
  bool ezOptionParser::gotValid (std::vector<std::string>& badOptions, std::vector<std::string>& badArgs)
  {
    std::vector<std::string> tempOptions;

    for (int i = 0; i < (int)groups.size(); ++i) {
      OptionGroup* g = groups[i];

      if (!g->validate (tempOptions)) {
        for (int j = 0; j < (int)tempOptions.size(); ++j) {
          badOptions.push_back (*g->flags[0]);
          badArgs.push_back (tempOptions[j]);
        }
      }

      tempOptions.clear();
    }

    return badOptions.empty();
  };
  /* ################################################################### */
  void ezOptionParser::parse (int argc, const char* argv[])
  {
    if (argc < 1) {
      return;
    }

    /*
    std::map<std::string,int>::iterator it;
    for ( it=optionGroupIds.begin() ; it != optionGroupIds.end(); it++ )
      std::cout << (*it).first << " => " << (*it).second << std::endl;
    */
    std::vector<std::string> args;
    unsigned int i;

    for (i = 0; i < (unsigned int)argc; i++) {
      args.push_back (argv[i]);
    }

    _progName = args.front();
    args.erase (args.begin());
    std::string s;
    OptionGroup* g;
    int k, lastOptIndex = 0;

    //parse args
    for (i = 0; i < args.size(); i++) {
      s = "";
      s = args[i];

      //long name with --
      if (s.substr (0, 2) == "--") {
        if (optionGroupIds.count (s)) {
          k = optionGroupIds[s];
          g = groups[k];
          g->isSet = 1;
          g->parseIndex.push_back (i);

          if (g->expectArgs) {
            ++i;

            if (i >= args.size()) {
              return;
            }

            g->args.push_back (new std::vector<std::string*>);
            SplitDelim (args[i], g->delim, g->args.back());
          }
        } else if (std::find (unknownArgs.begin(), unknownArgs.end(), args[i]) == unknownArgs.end()) {
          unknownArgs.push_back (args[i]);
        }

        lastOptIndex = i;
      } else if (s.substr (0, 1) == "-") {
        //args with - maybe combined
        s = s.substr (1); //remove -
        int length = s.size();

        for (int j = 0; j < length; ++j) {
          std::string letter = s.substr (j, 1);
          letter = "-" + letter;

          if (optionGroupIds.count (letter)) {
            k = optionGroupIds[letter];
            g = groups[k];
            g->isSet = 1;
            g->parseIndex.push_back (i);

            if (length == 1 && g->expectArgs) {
              ++i;

              if (i >= args.size()) {
                return;
              }

              g->args.push_back (new std::vector<std::string*>);
              SplitDelim (args[i], g->delim, g->args.back());
            }
          } else if (std::find (unknownArgs.begin(), unknownArgs.end(), letter) == unknownArgs.end()) {
            unknownArgs.push_back (letter);
          }
        }

        lastOptIndex = i;
      } else if (std::find (unknownArgs.begin(), unknownArgs.end(), args[i]) == unknownArgs.end()) {
        unknownArgs.push_back (args[i]);
      }
    }

    lastOptIndex++;// add the first arg

    for (k = lastOptIndex + 1; k < argc; ++k) {
      lastArgs.push_back (new std::string (argv[k]));
      std::vector<std::string>::iterator pos = std::find (unknownArgs.begin(), unknownArgs.end(), argv[k]);

      if (pos != unknownArgs.end()) {
        unknownArgs.erase (pos);
      }
    }
  };
  /* ################################################################### */
  void ezOptionParser::xorAdd (std::string a, std::string b)
  {
    std::vector<std::string> list;
    list.push_back (a);
    list.push_back (b);
    xorAdd (list);
  };
  /* ################################################################### */
  void ezOptionParser::xorAdd (std::vector<std::string>& list)
  {
    for (unsigned int i = 0; i < list.size(); i++) {
      if (optionGroupIds.count (list[i])) {
        int first = optionGroupIds[list[i]];
        xorGroups.push_back (groups[first]);
      }
    }
  };
  /* ################################################################### */
  bool ezOptionParser::checkValid (std::string& out)
  {
    bool result = true;
    std::vector<std::string> badOptions;

    //check required opitons
    if (!gotRequired (badOptions)) {
      for (int i = 0; i < (int)badOptions.size(); ++i) {
        out += "ERROR: Argument ";
        out += badOptions[i];
        out += " must be set.\n";
      }

      result = false;
    }

    badOptions.clear();

    // check opetions arguments setting right or not
    if (!gotExpected (badOptions)) {
      for (int i = 0; i < (int)badOptions.size(); ++i) {
        out += "ERROR: Got unexpected number of arguments for option ";
        out += badOptions[i];
        out += ".\n";
      }

      result = false;
    }

    badOptions.clear();
    //check validator
    std::vector<std::string> badArgs;

    if (!gotValid (badOptions, badArgs)) {
      for (int i = 0; i < (int)badOptions.size(); ++i) {
        out += "ERROR: Got invalid argument \"";
        out += badArgs[i];
        out += "\" for option ";
        out += badOptions[i];
        out += ".\n";
      }

      result = false;
    }

    //check xor OPTIONS
    int xorNum = 0;
    std::string xorstr;

    for (int i = 0; i < (int)xorGroups.size(); i++) {
      if (xorGroups[i]->isSet) {
        xorstr += " \"";
        xorstr += *xorGroups[i]->flags[0];
        xorstr += "\" ";
        xorNum++;
      }
    }

    if (xorNum > 1) {
      out += "ERROR:Can't set xor options at the same time:";
      out += xorstr;
      out += "\n";
      result  = false;
    }

    //check Unknown Options
    if (unknownArgs.size() > 0) {
      for (int i = 0; i < (int)unknownArgs.size(); ++i) {
        out += "WARNING: Got unknown argument:";
        out += unknownArgs[i];
        out += ".\n";
      }
    }

    if (!out.empty() && !result) {
      out += "\n";
      std::string usage;
      getUsage (usage);
      out += usage;
    }

    return result;
  };
  /* ################################################################### */
  void ezOptionParser::prettyPrint (std::string& out)
  {
    char tmp[256];
    int i, j, k;
    out += "First Args:\n";

    for (i = 0; i < (long int)firstArgs.size(); ++i) {
      sprintf (tmp, "%d: %s\n", i + 1, firstArgs[i]->c_str());
      out += tmp;
    }

    // Sort the option flag names.
    int n = groups.size();
    std::vector<std::string* > stringPtrs (n);

    for (i = 0; i < n; ++i) {
      stringPtrs[i] = groups[i]->flags[0];
    }

    // Sort first flag of each group with other groups.
    std::sort (stringPtrs.begin(), stringPtrs.end(), CmpOptStringPtr);
    out += "\nOptions:\n";
    OptionGroup* g;

    for (i = 0; i < n; ++i) {
      g = get (stringPtrs[i]->c_str());
      out += "\n";

      // The flag names:
      for (j = 0; j < (long int)g->flags.size() - 1; ++j) {
        sprintf (tmp, "%s, ", g->flags[j]->c_str());
        out += tmp;
      }

      sprintf (tmp, "%s:\n", g->flags.back()->c_str());
      out += tmp;

      if (g->isSet) {
        if (g->expectArgs) {
          if (g->args.empty()) {
            sprintf (tmp, "%s (default)\n", g->defaults.c_str());
            out += tmp;
          } else {
            for (k = 0; k < (long int)g->args.size(); ++k) {
              for (j = 0; j < (long int)g->args[k]->size() - 1; ++j) {
                sprintf (tmp, "%s%c", g->args[k]->at (j)->c_str(), g->delim);
                out += tmp;
              }

              sprintf (tmp, "%s\n", g->args[k]->back()->c_str());
              out += tmp;
            }
          }
        } else { // Set but no args expected.
          sprintf (tmp, "Set\n");
          out += tmp;
        }
      } else {
        sprintf (tmp, "Not set\n");
        out += tmp;
      }
    }

    out += "\nLast Args:\n";

    for (i = 0; i < (long int)lastArgs.size(); ++i) {
      sprintf (tmp, "%d: %s\n", i + 1, lastArgs[i]->c_str());
      out += tmp;
    }

    out += "\nUnknown Args:\n";

    for (i = 0; i < (long int)unknownArgs.size(); ++i) {
      sprintf (tmp, "%d: %s\n", i + 1, unknownArgs[i].c_str());
      out += tmp;
    }
  };
}
/* ################################################################### */
#endif /* EZ_OPTION_PARSER_H */