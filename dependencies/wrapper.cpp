#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include "XKeyboard.h"

using namespace std;

bool print_status(XKeyboard& xkb, string format) {
    stringstream r;     // resulting string

    for (size_t i = 0; i < format.length(); ++i) {
        if (i < format.length()-1 && format[i] == '%') {
            switch (format[i+1]) {
                case 's':
                    r << xkb.currentGroupSymbol();
                    break;
                default:
                    cerr << "Unknown format character: " << format[i+1] << endl;
                    return false;
                    break;
            }
        ++i;
		}
    }
    cout << r.str();
    return true;
}

bool set_group(XKeyboard& xkb, string group) {
    bool relative = false;

    // Check that 'group' is a valid integer (and whether it's relative or not)
    size_t i = 0;
    if (group[0] == '+' || group[0] == '-') {
        relative = true;
        ++i;
    }
    for (; i < group.length(); ++i) {
        if (!isdigit(group[i])) {
            cerr << group << " is not an integer" << endl;
            return false;
        }
    }

    int group_int = atoi(group.c_str());
    if (relative) {
        if (!xkb.changeGroup(group_int)) {
            cerr << "Failed to change group" << endl;
            return false;
        }
    } else {
        if (group_int >= xkb.groupCount()) {
            cerr << "layout_number must be between 0 and " << xkb.groupCount()-1 << endl;
            return false;
        } else {
            if (!xkb.setGroupByNum(group_int)) {
                cerr << "Failed to change group" << endl;
                return false;
            }
        }
    }
    return true;
}

int main(int argc, char* argv[])
{
    try {
        XKeyboard xkb;

        if (argc != 3) {
            return EXIT_FAILURE;
        } else {
            string command(argv[1]);
            if (command == "print") {
                if (!print_status(xkb, string(argv[2])))
                    return EXIT_FAILURE;
            } else if (command == "set") {
                if (!set_group(xkb, string(argv[2])))
                    return EXIT_FAILURE;
            } else {
                return EXIT_FAILURE;
            }
        }
    }
    catch (const exception *e) {
        cerr << e->what() << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

