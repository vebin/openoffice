/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/
#include "gConPo.hxx"
#include <iostream>
#include <fstream>
#include <cstdlib>



/*****************************************************************************
 **********************   G C O N P O W R A P . C X X   **********************
 *****************************************************************************
 * This includes the c code generated by flex
 *****************************************************************************/



/************   I N T E R F A C E   I M P L E M E N T A T I O N   ************/
convert_po::convert_po(l10nMem& crMemory)
                      : convert_gen_impl(crMemory),
                        mbExpectId(false),
                        mbExpectStr(false),
                        mbFuzzy(false)
{
  // Po files are handled special
  mbLoadMode = true;
}



/************   I N T E R F A C E   I M P L E M E N T A T I O N   ************/
convert_po::~convert_po()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
namespace PoWrap
{
#define IMPLptr convert_gen_impl::mcImpl
#define LOCptr ((convert_po *)convert_gen_impl::mcImpl)
#include "gConPo_yy.c"
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_po::startLook()
{
 std::string sFileName, sNewKey;
 int         i;


 if (!msKey.size() || !msId.size())
    return;

  // split key into filename and real key
  i = msKey.find("#");
  sFileName = msKey.substr(0, i);
  sNewKey   = msKey.substr(i+1);

  // load in db
  if (msId.size())
    mcMemory.loadEntryKey(miLineNo, sFileName, sNewKey, msId, msStr, mbFuzzy);

  // and prepare for new entry
  msKey.clear();
  msId.clear();
  msStr.clear();
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_po::setValue(char *syyText, int iLineCnt)
{
  if (mbExpectId)
    msId = syyText;
  if (mbExpectStr)
    msStr = syyText;
  mbExpectId  =
  mbExpectStr = false;
  miLineNo   += iLineCnt;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_po::setFuzzy()
{
  mbFuzzy = true;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_po::setKey(char *syyText)
{
  int i;


  // Activate "look for msg" mode.
  startLook();


  // skip "#:" and any blanks
  for (syyText += 2; *syyText == ' ' || *syyText == '\t'; ++syyText) ;
  msKey = syyText;

  // remove trailing blanks
  for (i = msKey.size() -1; msKey[i] == '\r' || msKey[i] == ' ' || msKey[i] == '\t'; --i) ;
  msKey.erase(i+1);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_po::setMsgId()
{
  mbExpectId = true;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_po::setMsgStr()
{
  mbExpectStr = true;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_po::handleNL()
{
  ++miLineNo;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_po::execute()
{
  PoWrap::yylex();
  startLook();
}

 
 
/**********************   I M P L E M E N T A T I O N   **********************/
void convert_po::startSave(const std::string& sLanguage,
                           const std::string& sFile)
{
  std::string sFilePath = msTargetPath + sLanguage + sFile;
  outBuffer.open(sFilePath.c_str(), std::ios::out);

  if (!outBuffer.is_open())
    throw l10nMem::showError("Cannot open " + sFilePath + " for writing");

  l10nMem::showDebug("writing file (" + sFilePath + ")");

  std::ostream outFile(&outBuffer);

  // Set license header
  outFile << "#*************************************************************"  << std::endl
          << "#*"                                                              << std::endl
          << "#* Licensed to the Apache Software Foundation (ASF) under one"   << std::endl
          << "#* or more contributor license agreements.  See the NOTICE file" << std::endl
          << "#* distributed with this work for additional information"        << std::endl
          << "#* regarding copyright ownership.  The ASF licenses this file"   << std::endl
          << "#* to you under the Apache License, Version 2.0 (the"            << std::endl
          << "#* \"License\"); you may not use this file except in compliance" << std::endl
          << "#* with the License.  You may obtain a copy of the License at"   << std::endl
          << "#*"                                                              << std::endl
          << "#*   http://www.apache.org/licenses/LICENSE-2.0"                 << std::endl
          << "#*"                                                              << std::endl
          << "#* Unless required by applicable law or agreed to in writing,"   << std::endl
          << "#* software distributed under the License is distributed on an"  << std::endl
          << "#* \"AS IS\" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY"     << std::endl
          << "#* KIND, either express or implied.  See the License for the"    << std::endl
          << "#* specific language governing permissions and limitations"      << std::endl
          << "#* under the License."                                           << std::endl
          << "#*"                                                              << std::endl
          << "#************************************************************"   << std::endl
          << "msgid \"\""                                                      << std::endl
          << "msgstr \"\""                                                     << std::endl
          << "\"Project-Id-Version: AOO-4-xx\\n\""                             << std::endl
          << "\"POT-Creation-Date: \\n\""                                      << std::endl
          << "\"PO-Revision-Date: \\n\""                                       << std::endl
          << "\"Last-Translator: genLang (build process)\\n\""                 << std::endl
          << "\"Language-Team: \\n\""                                          << std::endl
          << "\"MIME-Version: 1.0\\n\""                                        << std::endl
          << "\"Content-Type: text/plain; charset=UTF-8\\n\""                  << std::endl
          << "\"Content-Transfer-Encoding: 8bit\\n\""                          << std::endl
          << "\"X-Generator: genLang\\n\""                                     << std::endl
          << std::endl;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_po::save(const std::string& sFileName,
                      const std::string& sKey,
                      const std::string& sENUStext,
                      const std::string& sText,
                      bool               bFuzzy)
{
  std::ostream outFile(&outBuffer);
  
  outFile << std::endl << "#: " << sFileName << "#" << sKey << std::endl;
  if (bFuzzy)
    outFile << "#, fuzzy" << std::endl;
  outFile << "msgid  \"" << sENUStext << "\"" << std::endl
          << "msgstr \"" << sText     << "\"" << std::endl;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_po::endSave()
{
  outBuffer.close();
}
