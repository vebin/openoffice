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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_i18npool.hxx"

// prevent internal compiler error with MSVC6SP3
#include <utility>

#include <chaptercollator.hxx>
#include <com/sun/star/i18n/KCharacterType.hpp>
#include <com/sun/star/i18n/ParseResult.hpp>

using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::i18n;
using namespace ::rtl;

ChapterCollator::ChapterCollator( const Reference < XMultiServiceFactory >& rxMSF ) : CollatorImpl(rxMSF)
{
    if ( rxMSF.is()) {
        Reference < XInterface > xI =
		rxMSF->createInstance( OUString::createFromAscii("com.sun.star.i18n.CharacterClassification"));
        if ( xI.is() )
            xI->queryInterface(::getCppuType((const Reference< XCharacterClassification>*)0)) >>= cclass;
    }
}

ChapterCollator::~ChapterCollator()
{
}

sal_Int32 SAL_CALL
ChapterCollator::compareString( const OUString& s1, const OUString& s2) throw(RuntimeException)
{
    return compareSubstring(s1, 0, s1.getLength(),  s2, 0, s2.getLength());
}

#define DIGIT KCharacterType::DIGIT

sal_Int32 SAL_CALL
ChapterCollator::compareSubstring( const OUString& str1, sal_Int32 off1, sal_Int32 len1,
	const OUString& str2, sal_Int32 off2, sal_Int32 len2) throw(RuntimeException)
{
	if( len1 <= 1 || len2 <= 1 || ! cclass.is() )
	    return CollatorImpl::compareSubstring( str1, off1,  len1, str2, off2, len2 );

	sal_Int32 i1, i2;
	for (i1 = len1; i1 && (cclass->getCharacterType(str1, off1+i1-1, nLocale) & DIGIT); i1--) ;
	for (i2 = len2; i2 && (cclass->getCharacterType(str2, off2+i2-1, nLocale) & DIGIT); i2--) ;

	sal_Int32 ans = CollatorImpl::compareSubstring(str1, off1, i1, str2, off2, i2);
	if( ans != 0 )
	    return ans;

	const OUString &aAddAllowed = OUString::createFromAscii("?");
	ParseResult res1, res2;
	// Bug #100323#, since parseAnyToken does not take length as parameter, we have to copy
	// it to a temp. string.
	OUString s1 = str1.copy(off1+i1, len1-i1), s2 = str2.copy(off2+i2, len2-i2);
	res1 = cclass->parseAnyToken( s1, 0, nLocale, DIGIT, aAddAllowed, DIGIT, aAddAllowed );
	res2 = cclass->parseAnyToken( s2, 0, nLocale, DIGIT, aAddAllowed, DIGIT, aAddAllowed );

	return res1.Value == res2.Value ? 0 : res1.Value > res2.Value ? 1 : -1;
}

const sal_Char *cChapCollator = "com.sun.star.i18n.ChapterCollator";

OUString SAL_CALL
ChapterCollator::getImplementationName() throw( RuntimeException )
{
	return OUString::createFromAscii(cChapCollator);
}

sal_Bool SAL_CALL
ChapterCollator::supportsService(const rtl::OUString& rServiceName) throw( RuntimeException )
{
    return !rServiceName.compareToAscii(cChapCollator);
}

Sequence< OUString > SAL_CALL
ChapterCollator::getSupportedServiceNames() throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    aRet[0] = OUString::createFromAscii(cChapCollator);
    return aRet;
}
