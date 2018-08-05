#include "XKeyboard.h"
#include "X11Exception.h"
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <X11/XKBlib.h>

// XKeyboard -----------------------------------------------------------

XKeyboard::XKeyboard()
    : _display(0),
	  _groupCount(0),
	  _currentGroupNum(0),
      _deviceId(XkbUseCoreKbd) {

    XkbIgnoreExtension(False);

    char* displayName = strdup("");
    int eventCode;
    int errorReturn;
    int major = XkbMajorVersion;
    int minor = XkbMinorVersion;;
    int reasonReturn;
    _display = XkbOpenDisplay(displayName, &eventCode, &errorReturn, &major, &minor, &reasonReturn);
    switch (reasonReturn) {
    case XkbOD_BadLibraryVersion:
        throw X11Exception("Bad XKB library version.");
        break;
    case XkbOD_ConnectionRefused:
        throw X11Exception("Connection to X server refused.");
        break;
    case XkbOD_BadServerVersion:
        throw X11Exception("Bad X11 server version.");
        break;
    case XkbOD_NonXkbServer:
        throw X11Exception("XKB not present.");
        break;
    case XkbOD_Success:
        break;
    }

    if (initializeXkb() != True) {
        throw X11Exception("XKB not initialized.");
    }

    XkbSelectEventDetails(_display, XkbUseCoreKbd, XkbStateNotify, XkbAllStateComponentsMask, XkbGroupStateMask);

    XkbStateRec xkbState;
    XkbGetState(_display, _deviceId, &xkbState);
    _currentGroupNum = (_currentGroupNum != xkbState.group) ? xkbState.group : _currentGroupNum;
    accomodateGroupXkb();
}

Bool XKeyboard::initializeXkb() {
    // Initialize the XKB extension.
    int major = XkbMajorVersion;
    int minor = XkbMinorVersion;
    int opCode;
    /*Bool status =*/ XkbQueryExtension(_display, &opCode, &_baseEventCode, &_baseErrorCode,  &major, &minor);

    XkbDescRec* kbdDescPtr = XkbAllocKeyboard();
    if (kbdDescPtr == NULL) {
        std::cerr << "Failed to get keyboard description." << std::endl;
        return False;
    }

    kbdDescPtr->dpy = _display;
    if (_deviceId != XkbUseCoreKbd) {
        kbdDescPtr->device_spec = _deviceId;
    }

    XkbGetControls(_display, XkbAllControlsMask, kbdDescPtr);
    XkbGetNames(_display, XkbSymbolsNameMask, kbdDescPtr);
    XkbGetNames(_display, XkbGroupNamesMask, kbdDescPtr);

    if (kbdDescPtr->names == NULL) {
        std::cerr << "Failed to get keyboard description." << std::endl;
        return False;
    }

    // Count the number of configured groups.
    const Atom* groupSource = kbdDescPtr->names->groups;
    if (kbdDescPtr->ctrls != NULL) {
        _groupCount = kbdDescPtr->ctrls->num_groups;
    } else {
        _groupCount = 0;
        while (_groupCount < XkbNumKbdGroups &&
               groupSource[_groupCount] != None) {
            _groupCount++;
        }
    }

    // There is always at least one group.
    if (_groupCount == 0) {
        _groupCount = 1;
    }

    // Get the group names.
    const Atom* tmpGroupSource = kbdDescPtr->names->groups;
    Atom curGroupAtom;
    std::string groupName;
    for (int i = 0; i < _groupCount; i++) {
        if ((curGroupAtom = tmpGroupSource[i]) != None) {
            char* groupNameC = XGetAtomName(_display, curGroupAtom);
            if (groupNameC == NULL) {
                _groupNames.push_back("");
            } else {
                groupName = groupNameC;
                std::string::size_type pos = groupName.find('(', 0);
                if (pos != std::string::npos) {
                    groupName = groupName.substr(0, pos - 1);
                }
                _groupNames.push_back(groupName);
            }
            XFree(groupNameC);
        }
    }

    // Get the symbol name and parse it for layout symbols.
    Atom symNameAtom = kbdDescPtr->names->symbols;
    std::string symName;
    if (symNameAtom != None) {
        char* symNameC = XGetAtomName(_display, symNameAtom);
        symName = symNameC;
        XFree(symNameC);
        if (symName.empty()) {
            return False;
        }
    } else {
        return False;
    }

    XkbSymbolParser symParser;
    symParser.parse(symName, _symbolNames, _variantNames);
    int count = _symbolNames.size();
    if (count == 1 && _groupNames[0].empty()  && _symbolNames[0] == "jp") {
        _groupCount = 2;
        _symbolNames[1] = _symbolNames[0];
        _symbolNames[0] = "us";
        _groupNames[0] = "US/ASCII";
        _groupNames[1] = "Japanese";
    } else {
        if (count < _groupCount) {
            int j = count;
            int k = _groupCount;
            while (--j >= 0) _symbolNames[--k] = _symbolNames[j];
            while (--k >= 0) _symbolNames[k] = "en_US";
        }
    }

    count = _groupNames.size();
    for (int i = 0; i < count; i++) {
        if (_groupNames[i].empty()) {
            std::string name = getSymbolNameByResNum(i);
            if (name.empty()) {
                name = "U/A";
            }
            std::cerr << "Group Name " << i + 1 << " is undefined, set to '"
                      << name << "'!\n";
            _groupNames[i] = name;
        }
    }

    XkbStateRec xkbState;
    XkbGetState(_display, _deviceId, &xkbState);
    _currentGroupNum = xkbState.group;

    return True;
}

std::string XKeyboard::getSymbolNameByResNum(int groupResNum) {
    return _symbolNames[groupNumResToXkb(groupResNum)];
}

std::string XKeyboard::getGroupNameByResNum(int groupResNum) {
    return _groupNames[groupNumResToXkb(groupResNum)];
}


int XKeyboard::groupNumResToXkb(int groupResNum) {
    return groupLookup(groupResNum, _groupNames, _symbolNames, _groupCount);
}


int XKeyboard::groupLookup(int srcValue, StringVector fromText, StringVector toText, int count) {
    const std::string srcText = fromText[srcValue];

    if (!srcText.empty()) {
        std::string targetText;

        for (int i = 0; i < count; i++) {
            targetText = toText[i];
            if (compareNoCase(srcText, targetText) == 0) {
                srcValue = i;
                break;
            }
        }
    }
    return srcValue;
}

void XKeyboard::accomodateGroupXkb() {
    XkbStateRec state;
    XkbGetState(_display, _deviceId, &state);
    _currentGroupNum = state.group;
}


XKeyboard::~XKeyboard() {
    XCloseDisplay(_display);
    _display = NULL;
}

int XKeyboard::groupCount() const {
    return _groupCount;
}

StringVector XKeyboard::groupNames() const {
    return _groupNames;
}

StringVector XKeyboard::groupSymbols() const {
    return _symbolNames;
}

int XKeyboard::currentGroupNum() const {
    XkbStateRec xkbState;
    XkbGetState(_display, _deviceId, &xkbState);
    return static_cast<int>(xkbState.group);
}

std::string XKeyboard::currentGroupSymbol() const {
    return _symbolNames[currentGroupNum()];
}

bool XKeyboard::setGroupByNum(int groupNum) {
    if (_groupCount <= 1) {
        return false;
    }

    Bool result = XkbLockGroup(_display, _deviceId, groupNum);
    if (result == False) {
        return false;
    }
    accomodateGroupXkb();
    return true;
}

bool XKeyboard::changeGroup(int increment) {
    Bool result = XkbLockGroup(_display, _deviceId,
                               (_currentGroupNum + increment) % _groupCount);
    if (result == False) {
        return false;
    }
    accomodateGroupXkb();
    return true;
}


// XkbSymbolParser -----------------------------------------------------

XkbSymbolParser::XkbSymbolParser() {
    _nonSymbols.push_back("group");
    _nonSymbols.push_back("inet");
    _nonSymbols.push_back("pc");
}

XkbSymbolParser::~XkbSymbolParser() {
    _nonSymbols.clear();
}

void XkbSymbolParser::parse(const std::string& symbols, StringVector& symbolList, StringVector& variantList) {
    bool inSymbol = false;
    std::string curSymbol;
    std::string curVariant;

    for (size_t i = 0; i < symbols.size(); i++) {
        char ch = symbols[i];
        if (ch == '+' || ch == '_') {
            if (inSymbol) {
                if (isXkbLayoutSymbol(curSymbol)) {
                    symbolList.push_back(curSymbol);
                    variantList.push_back(curVariant);
                }
                curSymbol.clear();
                curVariant.clear();
            } else {
                inSymbol = true;
            }
        } else if (inSymbol && (isalpha(static_cast<int>(ch)) || ch == '_')) {
            curSymbol.append(1, ch);
        } else if (inSymbol && ch == '(') {
            while (++i < symbols.size()) {
                ch = symbols[i];
                if (ch == ')')
                    break;
                else
                    curVariant.append(1, ch);
            }
        } else {
            if (inSymbol) {
                if (isXkbLayoutSymbol(curSymbol)) {
                    symbolList.push_back(curSymbol);
                    variantList.push_back(curVariant);
                }
                curSymbol.clear();
                curVariant.clear();
                inSymbol = false;
            }
        }
    }

    if (inSymbol && !curSymbol.empty() && isXkbLayoutSymbol(curSymbol)) {
        symbolList.push_back(curSymbol);
        variantList.push_back(curVariant);
    }
}

bool XkbSymbolParser::isXkbLayoutSymbol(const std::string& symbol) {
    StringVectorIter result = find(_nonSymbols.begin(), _nonSymbols.end(), symbol);
    return result == _nonSymbols.end();
}

// Helper functions ----------------------------------------------------

int compareNoCase(const std::string& s1, const std::string& s2)
{
    std::string::const_iterator it1 = s1.begin();
    std::string::const_iterator it2 = s2.begin();

    //Has the end of at least one of the strings been reached?
    while (it1 != s1.end() && it2 != s2.end()) {
        // Do the letters differ?
        if (::toupper(*it1) != ::toupper(*it2)) {
            // return -1 to indicate 'smaller than', 1 otherwise
            return (::toupper(*it1) < ::toupper(*it2)) ? -1 : 1;
        }

        // Proceed to the next character in each string.
        ++it1;
        ++it2;
    }

    size_t size1 = s1.size();
    size_t size2 = s2.size();

    // Return -1, 0 or 1 according to strings' lengths.
    if (size1 == size2)  {
        return 0;
    }
    return (size1 < size2) ? -1 : 1;
}

