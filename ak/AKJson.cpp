#include "ak.h"
#include "AKJson.h"

namespace ak
{
    bool isInStr(int8 ch, int8 *str)
    {
        unsigned long i;
        int8 ch2;

        for (i = 0; (ch2 = str[i]) != '\0'; ++i)
        {
            if (ch == ch2)return true;
        }

        return false;
    }

    //是否有效字符
    bool isValid(int8 ch)
    {
        return !isInStr(ch," \t\r\n");
    }

    //是否数字
    bool isNumber(int8 ch)
    {
        if (ch >= '0' && ch <= '9')
            return true;
        else
            return false;
    }

    //是否字母
    bool isLetter(int8 ch)
    {
        if (ch >= 'a'&&ch <= 'z')return true;
        if (ch >= 'A'&&ch <= 'Z')return true;
        return false;
    }

    void strJoin(std::string &str, int8 ch)
    {
        int8 tmp[] = { ch,'\0' };
        str += tmp;
    }

    bool getWord(const int8 *str, int32 strSize, int &pos, AKJsonWord &word)
    {
        int32 c = strSize;
        for (int32 i = pos; i<c; ++i)
        {
            int8 ch = str[i];
            if (isValid(ch))
            {
                //跳过注释
                if (ch == '/')
                {
                    ch = str[i + 1];
                    switch (ch)
                    {
                        case '/'://行注释
                        {
                            for (i += 2;; ++i)
                            {
                                ch = str[i];

                                //源代码结束
                                if (ch == '\0')
                                {
                                    return false;
                                }

                                //注释结束
                                if (isInStr(ch, "\r\n"))
                                {
                                    pos = i + 1;
                                    break;
                                }
                            }
                            continue;
                            break;
                        }
                        case '*'://段注释
                        {
                            for (i += 2;; ++i)
                            {
                                ch = str[i];

                                //源代码结束
                                if (ch == '\0')
                                {
                                    pos = i;
                                    return false;
                                }
                                //注释结束
                                if (ch == '*'&&str[i + 1] == '/')
                                {
                                    ++i;
                                    pos = i + 1;
                                    break;
                                }
                            }
                            continue;
                            break;
                        }
                        default:
                        {
                            ch = '/';
                            break;
                        }
                    }
                }
                else if (ch == '+' || ch == '-' || ch == '.' || isNumber(ch))
                {
                    bool hasDot = false;
                    if (ch == '.')
                        hasDot = true;
                    std::string outStr;
                    strJoin(outStr, ch);
                    for (++i; i < c; ++i)
                    {
                        ch = str[i];
                        if (isNumber(ch))
                            strJoin(outStr, ch);
                        else if (ch == '.' && hasDot == false)
                            strJoin(outStr, ch);
                        else
                            break;
                    }
                    pos = i;
                    word.type = 1;
                    word.val = outStr;
                    return true;
                }
                else if (ch == '"')
                {
                    std::string outStr;
                    for (++i; i < c; ++i)
                    {
                        ch = str[i];
                        if (ch == '"')
                        {
                            pos = i + 1;
                            word.type = 2;
                            word.val = outStr;
                            return true;
                        }
                        else if (ch == '\\')
                        {
                            ++i;
                            ch = str[i];
                            if (ch == 't')
                                strJoin(outStr, '\t');
                            else if (ch == 'r')
                                strJoin(outStr, '\r');
                            else if (ch == 'n')
                                strJoin(outStr, '\n');
                            else
                                strJoin(outStr, ch);
                        }
                        else
                        {
                            strJoin(outStr, ch);
                        }
                    }
                    return false;
                }
                else if (ch == '_' || isLetter(ch) || ((uint8)ch) > 127)//以下划线/字母/汉字开头
                {
                    std::string outStr;
                    strJoin(outStr, ch);
                    for (++i; i < c; ++i)
                    {
                        ch = str[i];
                        if (!(ch == '_' || isLetter(ch) || ((uint8)ch) > 127 || isNumber(ch)))
                        {
                            break;
                        }
                        else
                        {
                            strJoin(outStr, ch);
                        }
                    }
                    pos = i;
                    word.type = 2;
                    word.val = outStr;
                    return true;
                }
                else if (ch == ',' || ch == '{' || ch == '}' || ch == ':' || ch == '[' || ch == ']')
                {
                    pos = i + 1;
                    word.type = 0;
                    std::string outStr;
                    strJoin(outStr, ch);
                    word.val = outStr;
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
        return false;
    }

    bool getObj(AKCore *core,AKVar *var,const int8 *str,int32 strSize,int &pos)
    {
        AKJsonWord word;
        if (getWord(str, strSize, pos, word))
        {
            int32 type = word.type;
            std::string &val = word.val;
            if (type == 0)//运算符
            {
                if (val == "[")
                {
                    container::AKArr *newArr = core->createArr();
                    var->setType(AKVar::typeHandle);
                    var->setValue((void *)newArr);
                    while (true)
                    {
                        int32 posOld = pos;
                        AKJsonWord word2;
                        if (getWord(str, strSize, pos, word2)==false)
                            return false;
                        if (word2.type == 0 && word2.val == "]")
                            break;
                        if (word2.type == 0 && word2.val == ",")
                            continue;
                        else if (word2.type == 0 && (word2.val == "[" || word2.val == "{"))
                        {
                            pos = posOld;
                            AKVar *newVar = new AKVar(core);
                            bool rt=getObj(core, newVar, str, strSize, pos);
                            newArr->mVarArray.push_back(newVar);
                            if (!rt)return false;
                        }
                        else if (word2.type == 1)
                            newArr->push(atof(word2.val.c_str()));
                        else if (word2.type == 2)
                            newArr->push(word2.val.c_str(), word2.val.size());
                        else
                            return false;//throw "json error";
                    }
                    return true;
                }
                else if (val == "{")
                {
                    container::AKDict *newDict = core->createDict();
                    var->setType(AKVar::typeHandle);
                    var->setValue((void *)newDict);
                    while (true)
                    {
                        AKJsonWord word2;
                        if (getWord(str, strSize, pos, word2)==false)
                            return false;
                        if (word2.type == 0 && word2.val == "}")
                            break;
                        if (word2.type == 0 && word2.val == ",")
                            continue;
                        else if (word2.type == 2)
                        {
                            AKJsonWord word3;
                            if (getWord(str, strSize, pos, word3) == false)
                                return false;//throw "json error";
                            if (!(word3.type == 0 && word3.val == ":"))
                                return false;//throw "json error";
                            int32 posOld = pos;
                            AKJsonWord word4;
                            if (getWord(str, strSize, pos, word4) == false)
                                return false;//throw "json error";
                            else if (word4.type == 0 && (word4.val == "[" || word4.val == "{"))
                            {
                                pos = posOld;
                                AKVar *newVar = new AKVar(core);
                                bool rt = getObj(core, newVar, str, strSize, pos);
                                const int8 *key = word2.val.c_str();
                                uint32 uintIndex;
                                if (AKCommon::string2UintIndex(key, uintIndex))
                                {
                                    newDict->mUint32VarMap[uintIndex] = newVar;
                                }
                                else
                                {
                                    newDict->mVarMap[key] = newVar;
                                }
                                if (!rt)return false;
                            }
                            else if (word4.type == 1)
                            {
                                newDict->set(word2.val.c_str(), atof(word4.val.c_str()));
                            }
                            else if (word4.type == 2)
                            {
                                newDict->set(word2.val.c_str(), word4.val.c_str());
                            }
                            else
                                return false;//throw "json error";
                        }
                        else
                            return false;//throw "json error";
                    }
                    return true;
                }
            }
            else if (type == 1)//数值
            {
                var->setType(AKVar::typeNumber);
                var->setValue(atof(val.c_str()));
                return true;
            }
            else if (type == 2)//字符串
            {
                var->setType(AKVar::typeString);
                var->setValue(val.c_str());
                return true;
            }
        }
        return false;
    }

    bool AKJson::decode(AKCore *core,AKVar *var, const int8 *str, int32 strSize)
    {
        int32 pos = 0;
        return getObj(core, var, str, strSize, pos);
    }
}
