from argparse import ONE_OR_MORE
from asyncore import write
from atexit import register
from gettext import find
from multiprocessing import context
from pydoc import describe
from sre_constants import IN
from turtle import pd
from unicodedata import name
import textract
import re
import PyPDF2

pre = "null"
def findall_index(pattern, text):
    #print("in findall")
    res =  [(m.start(0), m.end(0)) for m in re.finditer(pattern, text)]
    #print(res)
    return res
def checkSemicolon(index,text):
    # if index == (213,496):
    #     print(text[213:497])
    begin = index[0]
    end = index[1]
    trueBegin = begin
    trueEnd = 0
    for i in range(begin,end):
        if (text[i:i+6] == "return" or text[i:i+9] == "UNDEFINED" or text[i:i+24] == "AArch64.SystemAccessTrap"):
            trueBegin = i
            #print(text[trueBegin:trueBegin+10])
        if (text[i] == ";" and i > trueBegin):
            trueEnd = i+1;
    if trueEnd: #have ;
        return (trueBegin,trueEnd) #if not begin with return/undifine/trap, other situation will be saved all
    else: #no have; page foot
        return []
def subCondition(condition,text):#delete the conditions, leave the branch contexts
    #print(condition)
    res = []
    length = len(text)
    conditionNum = len(condition)
    if (not condition):
        single = checkSemicolon((0,length),text)
        if single:
            res.append(single)
        return res
    pre = condition[0]
    for cond in condition[1:]:
        if (cond[0]-pre[1] > 0):
            single = checkSemicolon((pre[1],cond[0]),text)
            if single:
                res.append(single)
        pre = cond

    single = checkSemicolon((condition[conditionNum-1][1],length),text)
    if single:
        res.append(single)    
    #print(res)
    return res

def newLine(text,beginI):
    begin = beginI
    end = 0
    #print(text[beginI:beginI+30])
    for i in range(beginI,beginI+250):
        # if (text[i] == ','):
        #     describeBegin = i
        if (text[i] == '\n' or text[i] == '\r'):
            end =  i
        if (begin and end):
            return text[begin:end]
def getName(text,beginI):
    describeBegin = beginI
    end = 0
    #print(text[beginI:beginI+30])
    for i in range(beginI,beginI+250):
        if (text[i] == ','):
            end = i
        if (describeBegin and end):
            return text[describeBegin:end]
def getBackInst(text,beginI):
    end = beginI
    begin = 0
    #print(text[beginI:beginI+30])
    for i in range(beginI,beginI-100,-1):
        if (text[i] == '\n' or text[i] == '\r'):
            begin =  i
        if (text[i-1:i+1] == ': '):
            begin = i
        if (text[i-3:i+1] == '2021'):
            begin = i
        if (text[i-11:i+1] == 'Confidential'):
            begin = i
        if (begin and end):
            return text[begin+1:end]
def getDescript(text,beginI,ignoreComma):
    begin = 0
    end = 0
    # print(text[beginI:beginI+150])
    for i in range(beginI, beginI+150):
        if (ignoreComma > 0 and text[i] == ','): 
            ignoreComma = ignoreComma - 1
        if (ignoreComma == 0): 
            begin = i + 1
            ignoreComma = -1
        if (text[i] == '\n' or text[i] == '\r'): 
            end = i
        if (begin and end): 
            return text[begin:end]
def findIns(pageNum,type,pdfReader,restxt,name,instName):
    #print(pageNum)
    global pre
    end = pageNum-50 if pageNum > 50 else 0
    for i in range(pageNum,end,-1):
        pageObj = pdfReader.getPage(i) 
        text = pageObj.extractText()
        Title = findall_index(r'(C5.[2-6].[0-9]+   )|(D13.[2-8].[0-9]+   )', text)
        if(Title):
            if (text[Title[0][0]:Title[0][0]+4] == 'C5.5'):# TLB maitenance
                descript = getDescript(text,Title[0][1],2)
            else:
                descript = getDescript(text,Title[0][1],1)
            if (text[Title[0][0]:Title[0][0]+4] != 'C5.3' and text[Title[0][0]:Title[0][0]+4] != 'C5.4' and text[Title[0][0]:Title[0][0]+4] != 'C5.5' and text[Title[0][0]:Title[0][0]+4] != 'C5.6'):
                titleName = getName(text,Title[0][1])
                if titleName != name and (name[len(name)-2:len(name)]!='02' and name[len(name)-2:len(name)]!='12') and i != 3605:return 1
            Rname = text[Title[0][0]:Title[0][1]] + str(instName) + str(descript)
            restxt.write(str(i+1))
            restxt.write(" ")
            restxt.write(Rname)
            restxt.write("\n")
            pre = Rname
            break
    return 0
    
    
def checkAllTrapOrUndefine(contextIndexs,branchText):
    for index in contextIndexs:
        #print(branchText[index[0]:index[1]])
        if branchText[index[0]:index[0]+9] != "UNDEFINED" and branchText[index[0]:index[0]+24] != "AArch64.SystemAccessTrap" :
            #print("in")
            return 0
    return 1
def checkHasTrap(contextIndexs,branchText):
    for index in contextIndexs:
        #print(branchText[index[0]:index[1]])
        if branchText[index[0]:index[0]+24] == "AArch64.SystemAccessTrap" :
            #print("in")
            return 1
    return 0
def checkHasUndef(contextIndexs,branchText):
    for index in contextIndexs:
        if branchText[index[0]:index[0]+9] == "UNDEFINED":
            return 1
    return 0
def checkOnlyUndefine(contextIndexs,branchText):
    for index in contextIndexs:
        #print(branchText[index[0]:index[1]])
        if branchText[index[0]:index[0]+9] != "UNDEFINED" :
            #print("in")
            return 0
    return 1

def parser(branchTexts):
    res = []
    for branchIndex in range(0,3):
        condition = findall_index(r'(if(.+?)then[ ]*)|(elsif(.+?)then[ ]*)|(else[ ]*)',branchTexts[branchIndex])
        contextIndexs = subCondition(condition,branchTexts[branchIndex])
        res.append(contextIndexs)
    return res
def parseOneMRS_MSR(branchTexts,pdfReader,i,EL02_equal):
    name = branchTexts[3]
    instName = branchTexts[4]
    branchContextIndexs = parser(branchTexts)#extract the true context indexs of every branch(undifined/trap/return xx/other)
    
    if (checkOnlyUndefine(branchContextIndexs[0],branchTexts[0]) and not checkOnlyUndefine(branchContextIndexs[2],branchTexts[2])):
        if (len(branchContextIndexs[0])!= 1): print(str(i+1) + " EL0:several undefine")
        # if (len(branchContextIndexs[2]) != 1): print(str(i+1) + " EL2:several undefine")
        findIns(i,0,pdfReader,EL02_equal,name,instName)

def readOneOrTwoPage(pdfReader,i,EL02_equal):
    pageObj = pdfReader.getPage(i) 
    text = pageObj.extractText()

    Pre = findall_index(r'(MRS <Xt>,)|(MSR(.+?), <Xt>)|(, <Xt>)|({, <Xt>})',text)
    # Pre = findall_index(r'MSR(.+?), <Xt>',text)
    originText = text
    for singleIS in Pre:
        if text[singleIS[0]:singleIS[0]+3] == 'MRS': 
            name = newLine(text,singleIS[1]+1)
            instName = newLine(text, singleIS[0])
        if text[singleIS[0]:singleIS[0]+3] == 'MSR':
            name = text[singleIS[0]+4:singleIS[1]-6]
            instName = text[singleIS[0]:singleIS[1]]
        if text[singleIS[0]] == '{' or text[singleIS[0]] == ',':
            name = getBackInst(text,singleIS[0])
            instName = name
        text = text[singleIS[1]:]
        Branch0 = findall_index(r'if PSTATE.EL == EL0 then[ ]*', text)
        Branch01 = findall_index(r'elsif Halted\(\) && ConstrainUnpredictableBool',text)
        Branch1 = findall_index(r'elsif PSTATE.EL == EL1 then[ ]*',text)
        Branch2 = findall_index(r'elsif PSTATE.EL == EL2 then[ ]*',text)
        Branch3 = findall_index(r'elsif PSTATE.EL == EL3 then[ ]*',text)
        
        if (not (Branch0 and Branch1 and Branch2 and Branch3)):
            pageObj = pdfReader.getPage(i) 
            text1 = pageObj.extractText()
            pageObj = pdfReader.getPage(i+1) 
            text2 = pageObj.extractText()
            text = text1 + text2
            text = text[singleIS[1]:]
            Branch0 = findall_index(r'if PSTATE.EL == EL0 then[ ]*', text)
            Branch01 = findall_index(r'elsif Halted\(\) && ConstrainUnpredictableBool',text)
            Branch1 = findall_index(r'elsif PSTATE.EL == EL1 then[ ]*',text)
            Branch2 = findall_index(r'elsif PSTATE.EL == EL2 then[ ]*',text)
            Branch3 = findall_index(r'elsif PSTATE.EL == EL3 then[ ]*',text)      
        getContext = 1
        if (not Branch0):
            print("DO not have EL0 " + str(i+1))
            getContext = 0
            text = originText
        if (not Branch1):
            print("Do not have EL1 " + str(i+1))
            getContext = 0
            text = originText
        if (not Branch2):
            print("Do not have EL2 " + str(i+1))
            getContext = 0
            text = originText
        if (not Branch3):
            print("Do not have EL3 " + str(i+1))
            getContext = 0
            text = originText

        if getContext:
            branch0Begin = Branch0[0][1]
            branch0End = Branch01[0][0] if (Branch01 and Branch01[0][0] < Branch1[0][0] and Branch01[0][0] > Branch0[0][1]) else Branch1[0][0]
            branch0Text = text[branch0Begin:branch0End]

            branch1Begin = Branch1[0][1]
            branch1End = Branch2[0][0]
            branch1Text = text[branch1Begin:branch1End]

            branch2Begin = Branch2[0][1]
            branch2End = Branch3[0][0]
            branch2Text = text[branch2Begin:branch2End]
            branchTexts = []
            branchTexts.append(branch0Text)
            branchTexts.append(branch1Text)
            branchTexts.append(branch2Text)
            branchTexts.append(name)
            branchTexts.append(instName)
            
            branchContextIndexs = parser(branchTexts)#extract the true context indexs of every branch(undifined/trap/return xx/other)
        else:
            branch0Text = "0"
            branch1Text = "1"
            branch2Text = "3"
            branchTexts = []
            branchContextIndexs = []
        if ((branch0Text == branch2Text) or (i == 439 or i == 501 or i == 3593 or i == 3597 or i == 3602 or i == 3604 or i == 3605 or i == 3668 or i == 3853 or i == 3861 or i == 4218)):
            a = 1
        elif ((branchContextIndexs and branchTexts and checkOnlyUndefine(branchContextIndexs[0],branchTexts[0])) or (i == 3595 or i == 3603 or i == 4140)):    
            findIns(i,0,pdfReader,EL02_equal,name,instName)
            # text = originText
            # continue
        # findIns(i,0,pdfReader,EL02_equal,name,instName)    
        # parseOneMRS_MSR(branchTexts,pdfReader,i,EL02_equal)
        text = originText

def parse_manual():
    
    # El0UnbehaviorTxt = open('MSR_EL0Trap(Undef).txt','w+',encoding='utf-8')
    # ElxBehaviorEqualTxt = open('MSR_ElxBehaviorEqual.txt','w+',encoding='utf-8')
    # ElxBehaviorDiffTxt = open('MSR_ElxBehaviorDiff.txt','w+',encoding='utf-8')
    # ElxTotalEqualTxt = open('MSR_ElxTotalEqual.txt','w+',encoding='utf-8')
    # EL12_EL02Txt = open('MSR_EL12_EL02.txt','w+',encoding='utf-8')
    # EL2_hasUndef = open('MSR_EL2_hasUndef.txt','w+', encoding='utf-8')
    EL02_equal = open('tmpResult.txt','w+', encoding='utf-8')
    pdfFileObj = open('DDI0487G_b_armv8_arm.pdf','rb')
    pdfReader = PyPDF2.PdfFileReader(pdfFileObj)
    
    # registers+C5 sys instr
    r = list(range(407,505)) + list(range(505, 870)) + list(range(3048,4244))
    
    # C5 sys instr
    # r = list(range(505, 868))

    for i in r:
        # if i != 3605: continue
        readOneOrTwoPage(pdfReader,i,EL02_equal)
                                        
parse_manual()





















