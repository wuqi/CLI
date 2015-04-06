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
                Support vc6,Clipping volume
*/
#ifndef EZ_OPTION_PARSER_H
#define EZ_OPTION_PARSER_H
//disable warnings
#if defined _MSC_VER &&  _MSC_VER<1400
#pragma warning(disable: 4786)
#endif
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <limits>
#include <sstream>
namespace ez
{
#define DEBUGLINE() printf("%s:%d\n", __FILE__, __LINE__);
  enum EZ_TYPE { EZ_NOTYPE = 0, EZ_BOOL, EZ_INT8, EZ_UINT8, EZ_INT16, EZ_UINT16, EZ_INT32, EZ_UINT32, EZ_INT64, EZ_UINT64, EZ_FLOAT, EZ_DOUBLE, EZ_TEXT };
  static const std::string EZ_TYPE_NAME[] = {"NOTYPE", "bool", "char", "unsigned char", "short", "unsigned short", "int", "unsigned int", "long", "unsigned long", "float", "double", "string"};
  static const char delim = ',';
  /* ################################################################### */
  // Variant that uses deep copies and references instead of pointers (less efficient).
  static void SplitDelim (const std::string& s, const char token, std::vector<std::string>& result)
  {
    std::stringstream ss (s);
    std::string item;

    while (std::getline (ss, item, token)) {
      result.push_back (item);
    }
  };
  class OptionGroup
  {
  public:
    OptionGroup() : expectArgs (0), isRequired (false), isSet (false) , needValidate (false), optType (EZ_NOTYPE), isUnlabeled (false) { }
    ~OptionGroup() {};
    std::string defaults;
    int expectArgs;
    std::string help;
    std::string argsFormat;
    bool isRequired;
    std::vector<std::string> flags;
    bool isSet;
    bool needValidate;
    std::vector<std::vector<std::string> > args;
    std::vector<std::string> validValues;
    std::string maxValue;
    std::string minValue;
    EZ_TYPE optType;
    bool isUnlabeled;
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
        if (args.empty() || args[0].empty()) {
          ss << "";
          ss >> out;
        } else {
          ss << args[0].at (0);
          ss >> out;
        }
      }
    };
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
        if (! (args.empty() || args[0].empty()))

          //StringsToInts(args[0], &out);
          for (int i = 0; i < (long int)args[0].size(); ++i) {
            ss << args[0].at (i);
            T temp;
            ss >> temp;
            out.push_back (temp);
            ss.clear();
          }
      }
    };
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
            for (int j = 0; j < (long int)args[i].size(); ++j) {
              ss << args[i].at (j);
              T temp;
              ss >> temp;
              out[i].push_back (temp);
              ss.clear();
            }
          }
        }
      }
    };
    inline bool validate (std::vector<std::string>& badOptions)
    {
      if (needValidate) {
        //need Validate
        //check type
        switch (optType) {
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
    template<typename T>
    inline bool validate (T example, std::vector<std::string>& badOptions)
    {
      std::stringstream ss;

      if (validValues.size() == 0) {
        //check range
        T mint = example, maxt = example;

        if (!minValue.empty()) {
          ss << minValue;
          ss >> mint;
          ss.clear();
        } else {
          mint = std::numeric_limits<T>::min();
        }

        if (!maxValue.empty()) {
          ss << maxValue;
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
          for (int j = 0; j < (int)args[i].size(); ++j) {
            ss << args[i].at (j);
            double value;
            ss >> value;
            ss.clear();
            ss.str ("");

            if (value > max || value < min) {
              badOptions.push_back (args[i].at (j));
            }
          }
        }
      } else {
        //check arguments whether in the valid value list
        for (int i = 0; i < (int)args.size(); ++i) {
          for (int j = 0; j < (int)args[i].size(); ++j) {
            std::string arg = args[i].at (j);

            if (std::find (validValues.begin(), validValues.end(), arg) == validValues.end()) {
              badOptions.push_back (args[i].at (j));
            }
          }
        }
      }

      return badOptions.empty();
    };
  };
  class ezOptionParser
  {
  public:
    ezOptionParser(): unlabeledNumber (0)
    {
      add ("", 0, 0, "Print this usage message", "-h,-help,--help,--usage");
    };
    inline ~ezOptionParser() {};
    //flags split by comma
    inline void add (const char* defaults, bool required, int expectArgs, const char* help, const char* flags, EZ_TYPE optType = EZ_NOTYPE, const std::string& minValue = std::string(), const std::string& maxValue = std::string(), const  char* validListStr = "")
    {
      int id = this->groups.size();
      OptionGroup g;
      g.defaults = defaults;
      g.isRequired = required;
      g.expectArgs = expectArgs;
      g.isSet = 0;

      if (optType != EZ_NOTYPE) {
        g.help.append ("[").append (EZ_TYPE_NAME[optType]).append ("]");
      }

      g.help.append (help);
      std::vector<std::string> flagsVector;
      SplitDelim (flags, delim, flagsVector);
      g.flags = flagsVector;
      g.optType = optType;
      g.minValue = minValue;
      g.maxValue = maxValue;
      std::vector<std::string> validList;
      SplitDelim (validListStr, delim, validList);
      g.validValues = validList;

      if (optType == EZ_TEXT && validList.size() > 0) {
        g.needValidate = true;
      } else if (optType != EZ_TEXT && optType != EZ_NOTYPE) {
        g.needValidate = true;
      }

      if (flagsVector[0].substr (0, 1) != "-") {
        //reset some properties
        g.isUnlabeled = true;
        g.isRequired = true;//option must be set if defined as unlabeled
        unlabeledPos.insert (std::pair<int , std::string> (unlabeledNumber, flagsVector[0]));
        unlabeledNumber++;
        g.argsFormat.append ("(");
      }

      if (g.expectArgs == 0) {
        g.argsFormat.append (" ");
      } else if (g.expectArgs == -1) {
        g.argsFormat.append (" Arg0,[ArgN] ");
      } else if (g.expectArgs == 1) {
        if (g.validValues.size() == 0) {
          g.argsFormat.append (" Arg ");
        } else {
          g.argsFormat.append (" ");

          for (int k = 0; k < (int)g.validValues.size(); ++k) {
            g.argsFormat.append (g.validValues[k]).append ("|");
          }
        }
      } else {
        std::stringstream ss;
        g.argsFormat.append (" ");

        for (int i = 0; i < g.expectArgs; i++) {
          std::string argstr;
          ss << i;
          ss >> argstr;
          ss.clear();
          g.argsFormat.append ("Arg").append (argstr).append (",");
        }
      }

      g.argsFormat.erase (g.argsFormat.size() - 1);

      if (g.isUnlabeled) {
        g.argsFormat.append (" ) ");
      }

      for (int i = 0; i < (int)flagsVector.size(); ++i) {
        this->optionGroupIds[flagsVector[i]] = id;
      }

      this->groups.push_back (g);
    };
    inline void xorAdd (const char* xorlistStr)
    {
      std::vector<std::string> list;
      SplitDelim (xorlistStr, delim, list);

      for (unsigned int i = 0; i < list.size(); i++) {
        if (optionGroupIds.count (list[i])) {
          xorGroups.push_back (list[i]);
        }
      }
    };
    inline OptionGroup get (const char* name)
    {
      if (optionGroupIds.count (name)) {
        return groups[optionGroupIds[name]];
      }

      return OptionGroup();
    };
    inline std::string getUsage()
    {
      std::string usage;
      std::stringstream ss;
      usage.append ("NAME: \n\n    ").append (overview).append ("\n\n").append ("USAGE: \n\n    ");
      bool xorset = false;
      int i, j, k;

      if (syntax.empty()) {
        syntax.append (_progName).append (" ");

        for (i = 0; i < (int)groups.size(); i++) {
          OptionGroup g = groups[i];
          std::string opt = " ";

          if ((std::find (xorGroups.begin(), xorGroups.end(), g.flags[0]) == xorGroups.end())) {
            //no xor
            if (!g.isRequired) {
              opt = "[";
            }

            opt.append (g.flags[0]).append (g.argsFormat);

            if (!g.isRequired) {
              opt.append ("] ");
            }
          } else if (!xorset) {
            opt = "";

            if (!g.isRequired) {
              opt = "[";
            }

            xorset = true;

            for (k = 0; k < (int)xorGroups.size(); ++k) {
              OptionGroup x = this->get (xorGroups[k].c_str());
              opt.append (x.flags[0]).append (x.argsFormat).append ("|");
            }

            opt.erase (opt.size() - 1); //rempve last ,

            if (!g.isRequired) {
              opt.append ("] ");
            }

            opt.append (" ");
          } else {
            continue;
          }

          syntax.append (opt);
        }
      }

      usage.append (syntax).append ("\n\nOPTIONS:\n\n");

      for (i = 0; i < (int)groups.size(); i++) {
        OptionGroup g = groups[i];
        usage.append ("    ");
        std::string flags;

        for (j = 0; j < (int)g.flags.size(); ++j) {
          flags.append (g.flags[j]).append (" ");
        }

        usage.append (flags).append (g.argsFormat).append (":\n").append ("        ").append (g.help).append ("\n");
      }

      if (!example.empty()) {
        usage.append ("\n\nEXAMPLES:\n\n    ").append (example);
      }

      if (!footer.empty()) {
        usage.append (footer);
      }

      return usage;
    };
    inline int isSet (const char* name)
    {
      std::string sname (name);

      if (this->optionGroupIds.count (sname)) {
        return this->groups[this->optionGroupIds[sname]].isSet;
      }

      return 0;
    };
    inline void parse (int argc, const char* argv[])
    {
      if (argc < 1) {
        return;
      }

      std::vector<std::string> args;
      int i, j, Id, unlabel = 0;

      for (i = 0; i < (int)argc; i++) {
        args.push_back (argv[i]);
      }

      _progName = args.front();
      args.erase (args.begin());
      std::string s;

      for (i = 0; i < (int) args.size(); i++) {
        s = args[i];

        if (s.substr (0, 1) == "-" && s.substr (0, 2) != "--") {
          //args with - , maybe combined
          s = s.substr (1);
          int length = s.size();

          for (j = 0; j < length; ++j) {
            std::string letter = s.substr (j, 1);
            letter = "-" + letter;

            if (optionGroupIds.count (letter)) {
              Id = optionGroupIds[letter];
              groups[Id].isSet = 1;

              if (groups[Id].expectArgs) {
                ++i;

                if (i >= (int)args.size()) {
                  return;
                }

                std::vector<std::string> argOptions;
                SplitDelim (args[i], delim, argOptions);
                groups[Id].args.push_back (argOptions);
              }
            } else if (std::find (unknownOptions.begin(), unknownOptions.end(), s) == unknownOptions.end()) {
              unknownOptions.push_back (s);
            }
          }
        } else if (s.substr (0, 2) == "--") {
          //long name single option
          if (optionGroupIds.count (s)) {
            Id = optionGroupIds[s];
            groups[Id].isSet = 1;

            if (groups[Id].expectArgs) {
              ++i;

              if (i >= (int)args.size()) {
                return;
              }

              std::vector<std::string> argOptions;
              SplitDelim (args[i], delim, argOptions);
              groups[Id].args.push_back (argOptions);
            }
          } else if (std::find (unknownOptions.begin(), unknownOptions.end(), s) == unknownOptions.end()) {
            unknownOptions.push_back (s);
          }
        } else {
          //unlabeled OptionGroup
          if (unlabeledPos.count (unlabel)) {
            s = unlabeledPos[unlabel];

            if (optionGroupIds.count (s)) {
              Id = optionGroupIds[s];
              groups[Id].isSet = 1;
              std::vector<std::string> argOptions;
              SplitDelim (args[i], delim, argOptions);
              groups[Id].args.push_back (argOptions);
            }

            unlabel++;
          } else if (std::find (unknownOptions.begin(), unknownOptions.end(), s) == unknownOptions.end()) {
            unknownOptions.push_back (s);
          }
        }
      }
    };
    inline bool checkValid (std::string& out)
    {
      int i, j;
      bool isValid = true;

      //check required
      for (i = 0; i < (int)groups.size(); ++i) {
        if (groups[i].isRequired && !groups[i].isSet) {
          out.append ("ERROR: Argument ").append (groups[i].flags[0]).append (" must be set.\n");
          isValid = false;
        }
      }

      //check expectArgs
      for (i = 0; i < (int)groups.size(); ++i) {
        OptionGroup g = groups[i];

        if (g.isSet) {
          if (g.expectArgs != 0 && g.args.empty()) {
            out.append ("ERROR: Got unexpected number of arguments for option ").append (g.flags[0]).append (".\n");
            isValid = false;
            continue;
          }

          for (j = 0; j < (int)g.args.size(); ++j) {
            if ((g.expectArgs != -1 && g.expectArgs != (int)g.args[j].size()) ||
                (g.expectArgs == -1 && g.args[j].size() == 0)) {
              out.append ("ERROR: Got unexpected number of arguments for option ").append (g.flags[0]).append (".\n");
              isValid = false;
            }
          }
        }
      }

      //check arguments validation
      std::vector<std::string> tempOptions;

      for (i = 0; i < (int)groups.size(); ++i) {
        if (!groups[i].validate (tempOptions)) {
          for (j = 0; j < (int)tempOptions.size(); ++j) {
            out.append ("ERROR: Got invalid argument \"").append (tempOptions[j]).append ("\" for option ").append (groups[i].flags[0]).append (".\n");
            isValid = false;
          }
        }

        tempOptions.clear();
      }

      //check xor groups
      int xorNum = 0;
      std::string xorstr;

      for (i = 0; i < (int)xorGroups.size(); i++) {
        if (this->get (xorGroups[i].c_str()).isSet) {
          xorstr.append (" \"").append (xorGroups[i]).append ("\" ");
          xorNum++;
        }
      }

      if (xorNum > 1) {
        out.append ("ERROR:Can't set xor options at the same time:").append (xorstr).append ("\n");
        isValid  = false;
      }

      //print unknown OPTIONS
      if ((int)unknownOptions.size() > 0) {
        for (i = 0; i < (int)unknownOptions.size(); ++i) {
          out.append ("WARNING: Got unknown argument:").append (unknownOptions[i]).append (".\n");
        }
      }

      //return out
      if (!out.empty() && !isValid) {
        out.append ("\n").append (getUsage());
      }

      return isValid;
    };
    //description
    std::string overview;
    std::string syntax;
    std::string example;
    std::string footer;
    std::string _progName;
    //members
    std::map< std::string, int > optionGroupIds;
    std::vector<OptionGroup> groups;
    std::vector<std::string> xorGroups;
    std::vector<std::string> unknownOptions;
    std::map<int, std::string> unlabeledPos; //unlabeled name and pos
  private:
    int unlabeledNumber;
  };
}
/* ################################################################### */
#endif /* EZ_OPTION_PARSER_H */