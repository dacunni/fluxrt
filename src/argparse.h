#ifndef __ARGPARSE_H__
#define __ARGPARSE_H__

#include <string>
#include <map>
#include <getopt.h>

class CommandLineArgumentParser
{
    public:
        CommandLineArgumentParser() = default;
        ~CommandLineArgumentParser() = default;

        using CallbackFn = std::function<void(const std::string &)>;

        void addFlag(int shortName, std::string longName);
        void addFlag(int shortName, std::string longName, bool & target);

        void addArgument(int shortName, std::string longName, CallbackFn callback);
        void addArgument(int shortName, std::string longName, int & target);
        void addArgument(int shortName, std::string longName, unsigned int & target);
        void addArgument(int shortName, std::string longName, float & target);
        void addArgument(int shortName, std::string longName, std::string & target);

        bool parse(int argc, char ** argv);

        bool getFlag(int shortName);
        std::string getArgument(int shortName);

        std::vector<std::string> unnamedArguments();

        void print();
        void printUsage();

    protected:
        struct Argument {
            int shortName;
            std::string longName;
            bool requiresArgument;
            CallbackFn callback;
            int flagValue;
            std::string value;

            struct option getoptParams;
        };
        std::vector<Argument>    _arguments;
        std::vector<std::string> _unnamedArguments;
        std::string              _getoptPattern;
        std::map<int, int>       _shortNameToArgumentIndex;
};

void CommandLineArgumentParser::addFlag(int shortName, std::string longName)
{
    auto callback = [](const std::string &) {};
    _arguments.push_back({shortName, longName, false, callback, 0, ""});
    _shortNameToArgumentIndex[shortName] = _arguments.size() - 1;

    auto & argument = _arguments.back();

    struct option opt = {
        "", // filled in by parse()
        no_argument,
        &_arguments.back().flagValue,
        1
    };

    argument.getoptParams = opt;

    _getoptPattern.push_back(char(shortName));
}

void CommandLineArgumentParser::addFlag(int shortName, std::string longName, bool & target)
{
    addFlag(shortName, longName);
    auto & argument = _arguments[_shortNameToArgumentIndex[shortName]];
    argument.callback = [&target](const std::string & s) { target = true; };
}

void CommandLineArgumentParser::addArgument(int shortName, std::string longName,
                                            CallbackFn callback)
{
    _arguments.push_back({shortName, longName, true, callback, false});
    _shortNameToArgumentIndex[shortName] = _arguments.size() - 1;

    auto & argument = _arguments.back();

    struct option opt = {
        "", // filled in by parse()
        required_argument,
        0,
        shortName
    };

    argument.getoptParams = opt;

    _getoptPattern.push_back(char(shortName));
    _getoptPattern.push_back(':');
}

void CommandLineArgumentParser::addArgument(int shortName, std::string longName, int & target)
{
    auto callback = [&target](const std::string & s) { target = std::stoi(s); };
    addArgument(shortName, longName, callback);
}

void CommandLineArgumentParser::addArgument(int shortName, std::string longName, unsigned int & target)
{
    auto callback = [&target](const std::string & s) { target = std::stoul(s); };
    addArgument(shortName, longName, callback);
}

void CommandLineArgumentParser::addArgument(int shortName, std::string longName, float & target)
{
    auto callback = [&target](const std::string & s) { target = std::stof(s); };
    addArgument(shortName, longName, callback);
}

void CommandLineArgumentParser::addArgument(int shortName, std::string longName, std::string & target)
{
    auto callback = [&target](const std::string & s) { target = s; };
    addArgument(shortName, longName, callback);
}

bool CommandLineArgumentParser::parse(int argc, char ** argv)
{
    std::vector<struct option> getoptTable;

    for(auto & argument : _arguments) {
        auto opt = argument.getoptParams;
        opt.name = argument.longName.data();
        getoptTable.push_back(opt);
    }
    getoptTable.push_back({0});

    while(true) {
        int optionIndex = 0;

        int c = getopt_long(argc, argv, _getoptPattern.c_str(),
                            &getoptTable[0], &optionIndex);

        if(c == -1) {
            break;
        }

        if(c == 0) {
            if(getoptTable[optionIndex].flag) {
                continue;
            }
        }

        auto & argument = _arguments[_shortNameToArgumentIndex[c]];

        if(optarg) {
            argument.value = optarg;
            argument.callback(argument.value);
        }
        else {
            argument.callback("");
        }

        argument.flagValue = true;
    }

    for(; optind < argc; ++optind) {
        _unnamedArguments.push_back(argv[optind]);
    }

    return true;
}

bool CommandLineArgumentParser::getFlag(int shortName)
{
    auto & argument = _arguments[_shortNameToArgumentIndex[shortName]];
    return argument.flagValue;
}

std::string CommandLineArgumentParser::getArgument(int shortName)
{
    auto & argument = _arguments[_shortNameToArgumentIndex[shortName]];
    return argument.value;
}

std::vector<std::string> CommandLineArgumentParser::unnamedArguments()
{
    return _unnamedArguments;
}

void CommandLineArgumentParser::print()
{
    printf("Options:\n");
    for(auto & option : _arguments) {
        printf("%10s : %c | %10s : flag %d value %s\n",
               option.requiresArgument ? "Argument" : "Flag",
               (char) option.shortName,
               option.longName.c_str(),
               option.flagValue,
               option.value.c_str());
    }

    printf("Unnamed:\n");
    for(auto & arg : _unnamedArguments) {
        printf("  %s\n", arg.c_str());
    }
}

void CommandLineArgumentParser::printUsage()
{
    printf("<program> <arguments>\n");

    for(auto & option : _arguments) {
        printf("    -%c --%s %s\n",
               (char) option.shortName,
               option.longName.c_str(),
               option.requiresArgument ? "<arg>" : ""
               );
    }
}

#endif
