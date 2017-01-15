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
#include "precompiled_scui.hxx"

#ifdef SC_DLLIMPLEMENTATION
#undef SC_DLLIMPLEMENTATION
#endif

#include <vcl/svapp.hxx>
#include <svl/aeitem.hxx>
#include <svl/stritem.hxx>
#include <svl/eitem.hxx>
#include <svl/intitem.hxx>
#include <basic/sbmeth.hxx>
#include <basic/sbstar.hxx>
#include <basic/sbmod.hxx>
#include <sfx2/app.hxx>

#include "scresid.hxx"
#include "sc.hrc"       // -> Slot IDs

#include "validat.hxx"
#include "validate.hrc"
#include "validate.hxx"
#include "compiler.hxx"
#include "formula/opcode.hxx" //CHINA001
//<!--Added by PengYunQuan for Validity Cell Range Picker
#include "tabvwsh.hxx" 
#include <sfx2/viewfrm.hxx>
#include <sfx2/childwin.hxx>
#include "reffact.hxx"
//-->Added by PengYunQuan for Validity Cell Range Picker

// ============================================================================

static sal_uInt16 pValueRanges[] =
{
	FID_VALID_MODE,	FID_VALID_ERRTEXT,
    FID_VALID_LISTTYPE, FID_VALID_LISTTYPE,
	0
};


// ============================================================================

//<!--Modified by PengYunQuan for Validity Cell Range Picker
//ScValidationDlg::ScValidationDlg( Window* pParent, const SfxItemSet* pArgSet ) :
//    SfxTabDialog( pParent, ScResId( TAB_DLG_VALIDATION ), pArgSet )
ScValidationDlg::ScValidationDlg( Window*			pParent,
								 const SfxItemSet* pArgSet,
								 ScTabViewShell *pTabViewSh,
								 SfxBindings *pB /*= NULL*/
					  ) :
		ScValidationDlgBase( pParent ? pParent : SFX_APP()->GetTopWindow(), TAB_DLG_VALIDATION, pArgSet, pB ),
			m_bOwnRefHdlr( false ),
			m_pTabVwSh( pTabViewSh ),
			m_bRefInputting( false )
//-->Modified by PengYunQuan for Validity Cell Range Picker
{
    AddTabPage( TP_VALIDATION_VALUES,    ScTPValidationValue::Create, 0 );
	AddTabPage( TP_VALIDATION_INPUTHELP, ScTPValidationHelp::Create,  0 );
    AddTabPage( TP_VALIDATION_ERROR,     ScTPValidationError::Create, 0 );
	FreeResource();
}

//<!--Added by PengYunQuan for Validity Cell Range Picker
void ScTPValidationValue::SetReferenceHdl( const ScRange&rRange , ScDocument* pDoc )
{
	if ( rRange.aStart != rRange.aEnd )
		if ( ScValidationDlg *pValidationDlg = GetValidationDlg() )
			if( m_pRefEdit ) 
				pValidationDlg/*->GetRefHandler()*/->RefInputStart( m_pRefEdit );

	if ( m_pRefEdit )
	{
		String	aStr;
		rRange.Format( aStr, SCR_ABS_3D, pDoc );
		m_pRefEdit->SetRefString( aStr );
		//m_pRefEdit->SetRefString( rRange.aStart != rRange.aEnd ? aStr : String::CreateFromAscii("=").Append( aStr ) );
	}
}

void ScTPValidationValue:: SetActiveHdl()
{
	if ( m_pRefEdit ) m_pRefEdit->GrabFocus();
	
	if ( ScValidationDlg *pValidationDlg = GetValidationDlg() )
		if( m_pRefEdit ) 
		{
			pValidationDlg/*->GetRefHandler()*/->RefInputDone();
		}
}

void			ScTPValidationValue::RefInputStartPreHdl( ScRefEdit* pEdit, ScRefButton* pButton )
{
	if ( ScValidationDlg *pValidationDlg = GetValidationDlg() )
	{
		if( pEdit == m_pRefEdit )
		{
			if( Window *pPreWnd = pEdit==&maEdMax?&maFtMax:(pEdit==&maEdMin?&maFtMin:NULL) )
			{
				pPreWnd->SetParent( pValidationDlg );
				pPreWnd->Hide();
			}

			m_pRefEdit->SetParent( pValidationDlg );
		}

		if( pButton == &m_btnRef )m_btnRef.SetParent( pValidationDlg );
	}
}

void			ScTPValidationValue::RefInputDonePreHdl()
{
	
	if( m_pRefEdit && m_pRefEdit->GetParent()!= this )
	{
		if( Window *pPreWnd = m_pRefEdit==&maEdMax?&maFtMax:(m_pRefEdit==&maEdMin?&maFtMin:NULL) )
		{
			pPreWnd->SetParent( this );
			pPreWnd->Show();
		}

		m_pRefEdit->SetParent( this );

		m_btnRef.SetParent( m_pRefEdit ); //if Edit SetParent but button not, the tab order will be incorrect, need button to setparent to anthor window and restore parent later in order to restore the tab order
//		aExample1.SetParent( m_pRefEdit ); // the aExample1's child order will affect acc key
	}
	
	if( m_btnRef.GetParent()!=this ) m_btnRef.SetParent( this );
//	if( aExample1.GetParent()!=this ) aExample1.SetParent( this );
}

void			ScTPValidationValue::RefInputDonePostHdl()
{
	
	
	if( m_pRefEdit && !m_pRefEdit->HasFocus() )
		m_pRefEdit->GrabFocus();

}
	

sal_Bool ScValidationDlg::Close()
{
	if( m_bOwnRefHdlr )
		if( SfxTabPage* pPage = GetTabPage( TP_VALIDATION_VALUES ) )
			static_cast<ScTPValidationValue*>(pPage)->RemoveRefDlg();

	return ScValidationDlgBase::Close();
}
//-->Added by PengYunQuan for Validity Cell Range Picker

ScValidationDlg::~ScValidationDlg()
{
	//<!--Added by PengYunQuan for Validity Cell Range Picker
	if( m_bOwnRefHdlr )
		RemoveRefDlg( sal_False );
	//-->Added by PengYunQuan for Validity Cell Range Picker
}


// ============================================================================

namespace {

/** Converts the passed ScValidationMode to the position in the list box. */
sal_uInt16 lclGetPosFromValMode( ScValidationMode eValMode )
{
    sal_uInt16 nLbPos = SC_VALIDDLG_ALLOW_ANY;
    switch( eValMode )
    {
        case SC_VALID_ANY:      nLbPos = SC_VALIDDLG_ALLOW_ANY;     break;
        case SC_VALID_WHOLE:    nLbPos = SC_VALIDDLG_ALLOW_WHOLE;   break;
        case SC_VALID_DECIMAL:  nLbPos = SC_VALIDDLG_ALLOW_DECIMAL; break;
        case SC_VALID_DATE:     nLbPos = SC_VALIDDLG_ALLOW_DATE;    break;
        case SC_VALID_TIME:     nLbPos = SC_VALIDDLG_ALLOW_TIME;    break;
        case SC_VALID_TEXTLEN:  nLbPos = SC_VALIDDLG_ALLOW_TEXTLEN; break;
        case SC_VALID_LIST:     nLbPos = SC_VALIDDLG_ALLOW_RANGE;   break;
        case SC_VALID_CUSTOM:   nLbPos = SC_VALIDDLG_ALLOW_ANY;     break;  // not supported
        default:    DBG_ERRORFILE( "lclGetPosFromValMode - unknown validity mode" );
    }
    return nLbPos;
}

/** Converts the passed list box position to an ScValidationMode. */
ScValidationMode lclGetValModeFromPos( sal_uInt16 nLbPos )
{
    ScValidationMode eValMode = SC_VALID_ANY;
    switch( nLbPos )
    {
        case SC_VALIDDLG_ALLOW_ANY:     eValMode = SC_VALID_ANY;        break;
        case SC_VALIDDLG_ALLOW_WHOLE:   eValMode = SC_VALID_WHOLE;      break;
        case SC_VALIDDLG_ALLOW_DECIMAL: eValMode = SC_VALID_DECIMAL;    break;
        case SC_VALIDDLG_ALLOW_DATE:    eValMode = SC_VALID_DATE;       break;
        case SC_VALIDDLG_ALLOW_TIME:    eValMode = SC_VALID_TIME;       break;
        case SC_VALIDDLG_ALLOW_RANGE:   eValMode = SC_VALID_LIST;       break;
        case SC_VALIDDLG_ALLOW_LIST:    eValMode = SC_VALID_LIST;       break;
        case SC_VALIDDLG_ALLOW_TEXTLEN: eValMode = SC_VALID_TEXTLEN;    break;
        default:    DBG_ERRORFILE( "lclGetValModeFromPos - invalid list box position" );
    }
    return eValMode;
}

/** Converts the passed ScConditionMode to the position in the list box. */
sal_uInt16 lclGetPosFromCondMode( ScConditionMode eCondMode )
{
    sal_uInt16 nLbPos = SC_VALIDDLG_DATA_EQUAL;
    switch( eCondMode )
    {
        case SC_COND_NONE:          // #111771# may occur in old XML files after Excel import
        case SC_COND_EQUAL:         nLbPos = SC_VALIDDLG_DATA_EQUAL;        break;
        case SC_COND_LESS:          nLbPos = SC_VALIDDLG_DATA_LESS;         break;
        case SC_COND_GREATER:       nLbPos = SC_VALIDDLG_DATA_GREATER;      break;
        case SC_COND_EQLESS:        nLbPos = SC_VALIDDLG_DATA_EQLESS;       break;
        case SC_COND_EQGREATER:     nLbPos = SC_VALIDDLG_DATA_EQGREATER;    break;
        case SC_COND_NOTEQUAL:      nLbPos = SC_VALIDDLG_DATA_NOTEQUAL;     break;
        case SC_COND_BETWEEN:       nLbPos = SC_VALIDDLG_DATA_BETWEEN;      break;
        case SC_COND_NOTBETWEEN:    nLbPos = SC_VALIDDLG_DATA_NOTBETWEEN;   break;
        default:    DBG_ERRORFILE( "lclGetPosFromCondMode - unknown condition mode" );
    }
    return nLbPos;
}

/** Converts the passed list box position to an ScConditionMode. */
ScConditionMode lclGetCondModeFromPos( sal_uInt16 nLbPos )
{
    ScConditionMode eCondMode = SC_COND_EQUAL;
    switch( nLbPos )
    {
        case SC_VALIDDLG_DATA_EQUAL:        eCondMode = SC_COND_EQUAL;      break;
        case SC_VALIDDLG_DATA_LESS:         eCondMode = SC_COND_LESS;       break;
        case SC_VALIDDLG_DATA_GREATER:      eCondMode = SC_COND_GREATER;    break;
        case SC_VALIDDLG_DATA_EQLESS:       eCondMode = SC_COND_EQLESS;     break;
        case SC_VALIDDLG_DATA_EQGREATER:    eCondMode = SC_COND_EQGREATER;  break;
        case SC_VALIDDLG_DATA_NOTEQUAL:     eCondMode = SC_COND_NOTEQUAL;   break;
        case SC_VALIDDLG_DATA_BETWEEN:      eCondMode = SC_COND_BETWEEN;    break;
        case SC_VALIDDLG_DATA_NOTBETWEEN:   eCondMode = SC_COND_NOTBETWEEN; break;
        default:    DBG_ERRORFILE( "lclGetCondModeFromPos - invalid list box position" );
    }
    return eCondMode;
}

/** Converts line feed separated string to a formula with strings separated by semicolons.
    @descr  Keeps all empty strings.
    Example: abc\ndef\n\nghi -> "abc";"def";"";"ghi".
    @param rFmlaStr  (out-param) The converted formula string. */
void lclGetFormulaFromStringList( String& rFmlaStr, const String& rStringList, sal_Unicode cFmlaSep )
{
    rFmlaStr.Erase();
    xub_StrLen nTokenCnt = rStringList.GetTokenCount( '\n' );
    for( xub_StrLen nToken = 0, nStringIx = 0; nToken < nTokenCnt; ++nToken )
    {
        String aToken( rStringList.GetToken( 0, '\n', nStringIx ) );
        ScGlobal::AddQuotes( aToken, '"' );
        ScGlobal::AddToken( rFmlaStr, aToken, cFmlaSep );
    }
    if( !rFmlaStr.Len() )
        rFmlaStr.AssignAscii( "\"\"" );
}


/** Converts formula with strings separated by semicolons to line feed separated string.
    @descr  Keeps all empty strings. Ignores all empty tokens (multiple semicolons).
    Example: "abc";;;"def";"";"ghi" -> abc\ndef\n\nghi.
    @param rStringList  (out-param) The converted line feed separated string list.
    @return  true = Conversion successful. */
bool lclGetStringListFromFormula( String& rStringList, const String& rFmlaStr, sal_Unicode cFmlaSep )
{
    String aQuotes( RTL_CONSTASCII_USTRINGPARAM( "\"\"" ) );
    xub_StrLen nTokenCnt = rFmlaStr.GetQuotedTokenCount( aQuotes, cFmlaSep );

    rStringList.Erase();
    bool bIsStringList = (nTokenCnt > 0);
    bool bTokenAdded = false;

    for( xub_StrLen nToken = 0, nStringIx = 0; bIsStringList && (nToken < nTokenCnt); ++nToken )
    {
        String aToken( rFmlaStr.GetQuotedToken( 0, aQuotes, cFmlaSep, nStringIx ) );
        aToken.EraseLeadingAndTrailingChars();
        if( aToken.Len() )      // ignore empty tokens, i.e. "a";;"b"
        {
            bIsStringList = ScGlobal::IsQuoted( aToken, '"' );
            if( bIsStringList )
            {
                ScGlobal::EraseQuotes( aToken, '"' );
                ScGlobal::AddToken( rStringList, aToken, '\n', 1, bTokenAdded );
                bTokenAdded = true;
            }
        }
    }

    return bIsStringList;
}

} // namespace

// ----------------------------------------------------------------------------

ScTPValidationValue::ScTPValidationValue( Window* pParent, const SfxItemSet& rArgSet ) :
    SfxTabPage( pParent, ScResId( TP_VALIDATION_VALUES ), rArgSet ),
    maFtAllow ( this, ScResId( FT_ALLOW ) ),
    maLbAllow ( this, ScResId( LB_ALLOW ) ),
    maCbAllow ( this, ScResId( TSB_ALLOW_BLANKS ) ),
    maCbShow  ( this, ScResId( CB_SHOWLIST ) ),
    maCbSort  ( this, ScResId( CB_SORTLIST ) ),
    maFtValue ( this, ScResId( FT_VALUE ) ),
    maLbValue ( this, ScResId( LB_VALUE ) ),
    maFtMin   ( this, ScResId( FT_MIN ) ),
    maEdMin   ( this, ScResId( EDT_MIN ) ),
    maEdList  ( this, ScResId( EDT_LIST ) ),
    maFtMax   ( this, ScResId( FT_MAX ) ),
    maEdMax   ( this, ScResId( EDT_MAX ) ),
    maFtHint  ( this, ScResId( FT_SOURCEHINT ) ),
    maStrMin  ( ScResId( SCSTR_VALID_MINIMUM ) ),
    maStrMax  ( ScResId( SCSTR_VALID_MAXIMUM ) ),
    maStrValue( ScResId( SCSTR_VALID_VALUE ) ),
    maStrRange( ScResId( SCSTR_VALID_RANGE ) ),
    maStrList ( ScResId( SCSTR_VALID_LIST ) ),
//<!--Added by PengYunQuan for Validity Cell Range Picker
		m_btnRef( this, ScResId( RB_VALIDITY_REF ) )
//-->Added by PengYunQuan for Validity Cell Range Picker
{
	Init();
	FreeResource();

    // list separator in formulas
    //CHINA001 const String& rListSep = ScCompiler::pSymbolTableNative[ ocSep ];
	String aListSep = ::GetScCompilerNativeSymbol( ocSep ); //CHINA001 
    DBG_ASSERT( aListSep.Len() == 1, "ScTPValidationValue::ScTPValidationValue - list separator error" );
    mcFmlaSep = aListSep.Len() ? aListSep.GetChar( 0 ) : ';';
	//<!--Added by PengYunQuan for Validity Cell Range Picker
	m_btnRef.Hide();
	//-->Added by PengYunQuan for Validity Cell Range Picker
}

ScTPValidationValue::~ScTPValidationValue()
{
}

void ScTPValidationValue::Init()
{
    maLbAllow.SetSelectHdl( LINK( this, ScTPValidationValue, SelectHdl ) );
    maLbValue.SetSelectHdl( LINK( this, ScTPValidationValue, SelectHdl ) );
    maCbShow.SetClickHdl( LINK( this, ScTPValidationValue, CheckHdl ) );
	//<!--Added by PengYunQuan for Validity Cell Range Picker
	maEdMin.SetGetFocusHdl( LINK( this, ScTPValidationValue, EditSetFocusHdl ) );
	maEdMin.SetLoseFocusHdl( LINK( this, ScTPValidationValue, KillFocusHdl ) );
	maEdMax.SetGetFocusHdl( LINK( this, ScTPValidationValue, EditSetFocusHdl ) );
	m_btnRef.SetLoseFocusHdl( LINK( this, ScTPValidationValue, KillFocusHdl ) );
	maEdMax.SetLoseFocusHdl( LINK( this, ScTPValidationValue, KillFocusHdl ) );
	//-->Added by PengYunQuan for Validity Cell Range Picker

    maLbAllow.SelectEntryPos( SC_VALIDDLG_ALLOW_ANY );
    maLbValue.SelectEntryPos( SC_VALIDDLG_DATA_EQUAL );

    SelectHdl( NULL );
    CheckHdl( NULL );
}

SfxTabPage* ScTPValidationValue::Create( Window* pParent, const SfxItemSet& rArgSet )
{
    return( new ScTPValidationValue( pParent, rArgSet ) );
}

sal_uInt16* ScTPValidationValue::GetRanges()
{
    return pValueRanges;
}

void ScTPValidationValue::Reset( const SfxItemSet& rArgSet )
{
	const SfxPoolItem* pItem;

    sal_uInt16 nLbPos = SC_VALIDDLG_ALLOW_ANY;
    if( rArgSet.GetItemState( FID_VALID_MODE, sal_True, &pItem ) == SFX_ITEM_SET )
        nLbPos = lclGetPosFromValMode( static_cast< ScValidationMode >(
            static_cast< const SfxAllEnumItem* >( pItem )->GetValue() ) );
    maLbAllow.SelectEntryPos( nLbPos );

    nLbPos = SC_VALIDDLG_DATA_EQUAL;
    if( rArgSet.GetItemState( FID_VALID_CONDMODE, sal_True, &pItem ) == SFX_ITEM_SET )
        nLbPos = lclGetPosFromCondMode( static_cast< ScConditionMode >(
            static_cast< const SfxAllEnumItem* >( pItem )->GetValue() ) );
    maLbValue.SelectEntryPos( nLbPos );

    // *** check boxes ***
    sal_Bool bCheck = sal_True;
    if( rArgSet.GetItemState( FID_VALID_BLANK, sal_True, &pItem ) == SFX_ITEM_SET )
        bCheck = static_cast< const SfxBoolItem* >( pItem )->GetValue();
    maCbAllow.Check( bCheck );

    sal_Int32 nListType = ValidListType::UNSORTED;
    if( rArgSet.GetItemState( FID_VALID_LISTTYPE, sal_True, &pItem ) == SFX_ITEM_SET )
        nListType = static_cast< const SfxInt16Item* >( pItem )->GetValue();
    maCbShow.Check( nListType != ValidListType::INVISIBLE );
    maCbSort.Check( nListType == ValidListType::SORTEDASCENDING );

    // *** formulas ***
    String aFmlaStr;
	if ( rArgSet.GetItemState( FID_VALID_VALUE1, sal_True, &pItem ) == SFX_ITEM_SET )
        aFmlaStr = static_cast< const SfxStringItem* >( pItem )->GetValue();
    SetFirstFormula( aFmlaStr );

    aFmlaStr.Erase();
	if ( rArgSet.GetItemState( FID_VALID_VALUE2, sal_True, &pItem ) == SFX_ITEM_SET )
        aFmlaStr = static_cast< const SfxStringItem* >( pItem )->GetValue();
    SetSecondFormula( aFmlaStr );

    SelectHdl( NULL );
    CheckHdl( NULL );
}

sal_Bool ScTPValidationValue::FillItemSet( SfxItemSet& rArgSet )
{
    sal_Int16 nListType = maCbShow.IsChecked() ?
        (maCbSort.IsChecked() ? ValidListType::SORTEDASCENDING : ValidListType::UNSORTED) :
        ValidListType::INVISIBLE;

    rArgSet.Put( SfxAllEnumItem( FID_VALID_MODE, sal::static_int_cast<sal_uInt16>(
                    lclGetValModeFromPos( maLbAllow.GetSelectEntryPos() ) ) ) );
    rArgSet.Put( SfxAllEnumItem( FID_VALID_CONDMODE, sal::static_int_cast<sal_uInt16>(
                    lclGetCondModeFromPos( maLbValue.GetSelectEntryPos() ) ) ) );
    rArgSet.Put( SfxStringItem( FID_VALID_VALUE1, GetFirstFormula() ) );
    rArgSet.Put( SfxStringItem( FID_VALID_VALUE2, GetSecondFormula() ) );
    rArgSet.Put( SfxBoolItem( FID_VALID_BLANK, maCbAllow.IsChecked() ) );
    rArgSet.Put( SfxInt16Item( FID_VALID_LISTTYPE, nListType ) );
	return sal_True;
}

String ScTPValidationValue::GetFirstFormula() const
{
    String aFmlaStr;
    if( maLbAllow.GetSelectEntryPos() == SC_VALIDDLG_ALLOW_LIST )
        lclGetFormulaFromStringList( aFmlaStr, maEdList.GetText(), mcFmlaSep );
    else
        aFmlaStr = maEdMin.GetText();
    return aFmlaStr;
}

String ScTPValidationValue::GetSecondFormula() const
{
    return maEdMax.GetText();
}

void ScTPValidationValue::SetFirstFormula( const String& rFmlaStr )
{
    // try if formula is a string list, validation mode must already be set
    String aStringList;
    if( (maLbAllow.GetSelectEntryPos() == SC_VALIDDLG_ALLOW_RANGE) &&
        lclGetStringListFromFormula( aStringList, rFmlaStr, mcFmlaSep ) )
    {
        maEdList.SetText( aStringList );
        maEdMin.SetText( EMPTY_STRING );
        // change validation mode to string list
        maLbAllow.SelectEntryPos( SC_VALIDDLG_ALLOW_LIST );
    }
    else
    {
        maEdMin.SetText( rFmlaStr );
        maEdList.SetText( EMPTY_STRING );
    }
}

void ScTPValidationValue::SetSecondFormula( const String& rFmlaStr )
{
    maEdMax.SetText( rFmlaStr );
}

//<!--Added by PengYunQuan for Validity Cell Range Picker
ScValidationDlg * ScTPValidationValue::GetValidationDlg()
{
	if( Window *pParent = GetParent() )
		do{
			if ( dynamic_cast<ScValidationDlg*>( pParent ) )
				return static_cast<	ScValidationDlg * >( pParent );
		}while ( NULL != ( pParent = pParent->GetParent() ) );
	return NULL;
}
void ScTPValidationValue::SetupRefDlg()
{
	if( ScValidationDlg *pValidationDlg = GetValidationDlg() )
		if( pValidationDlg->SetupRefDlg() )
		{
			pValidationDlg->SetHandler( this );
			pValidationDlg->SetSetRefHdl( (ScRefHandlerHelper::PFUNCSETREFHDLTYPE)( &ScTPValidationValue::SetReferenceHdl ) );
			pValidationDlg->SetSetActHdl( (ScRefHandlerHelper::PCOMMONHDLTYPE)( &ScTPValidationValue::SetActiveHdl ) );
			pValidationDlg->SetRefInputStartPreHdl( (ScRefHandlerHelper::PINPUTSTARTDLTYPE)( &ScTPValidationValue::RefInputStartPreHdl ) );
			pValidationDlg->SetRefInputDonePreHdl( (ScRefHandlerHelper::PCOMMONHDLTYPE)( &ScTPValidationValue::RefInputDonePreHdl ) );
			pValidationDlg->SetRefInputDonePostHdl( (ScRefHandlerHelper::PCOMMONHDLTYPE)( &ScTPValidationValue::RefInputDonePostHdl ) );

			if ( maEdMax.IsVisible() ) { m_pRefEdit = &maEdMax; }
			else if ( maEdMin.IsVisible() ) {  m_pRefEdit = &maEdMin; }

			if( m_pRefEdit && !m_pRefEdit->HasFocus() ) m_pRefEdit->GrabFocus();

			if( m_pRefEdit ) m_pRefEdit->SetRefDialog( pValidationDlg );
			m_btnRef.SetReferences( pValidationDlg, m_pRefEdit );
		}
}

void ScTPValidationValue::RemoveRefDlg()
{
	if( ScValidationDlg *pValidationDlg = GetValidationDlg() )
		if( pValidationDlg->RemoveRefDlg() )
		{
			pValidationDlg->SetHandler( NULL );
			pValidationDlg->SetSetRefHdl( NULL );
			pValidationDlg->SetSetActHdl( NULL );
			pValidationDlg->SetRefInputStartPreHdl( NULL );
			pValidationDlg->SetRefInputDonePreHdl( NULL );
			pValidationDlg->SetRefInputDonePostHdl( NULL );

			if( m_pRefEdit ) m_pRefEdit->SetRefDialog( NULL );
			m_pRefEdit = NULL;

			m_btnRef.SetReferences( NULL, NULL );

#if ! defined( WNT ) && !defined( _MSC_VER )
			TidyListBoxes();
#endif
		}
}

void ScTPValidationValue::TidyListBoxes()
{
	if ( Window *pWnd = GetChild( 0 ) )
	{
		bool bFindLst = false;
		std::list<Window*> alstOrder;

		do{
			if( pWnd->GetParent() == this )
			{
				if ( !bFindLst )
				{
					try{
						if( dynamic_cast<ListBox*>(pWnd)||dynamic_cast<ListBox*>(pWnd->GetWindow(WINDOW_CLIENT) ) )
							bFindLst = true;
					}
					catch( ... )
					{
						if ( *(void**)pWnd == *(void**)&maLbValue )
							bFindLst = true;
						else if ( Window *pClient = pWnd->GetWindow( WINDOW_CLIENT ) )
							if ( *(void**)pClient == *(void**)&maLbValue )
								bFindLst = true;
					}
				}

				if ( bFindLst )
					alstOrder.push_back( pWnd->GetWindow( WINDOW_CLIENT ) );
			}
		}while( NULL != ( pWnd = pWnd->GetWindow( WINDOW_NEXT  ) ) );

		pWnd = GetChild(0);

		while( std::find( alstOrder.begin(), alstOrder.end(), pWnd ) != alstOrder.end() && NULL != ( pWnd = pWnd->GetWindow( WINDOW_NEXT) ) ) ; 

		if ( pWnd )
		{
			for ( std::list<Window*>::iterator i = alstOrder.begin(); i!=alstOrder.end(); i++ )
			{
				Window *pParent = (*i)->GetParent();
				(*i)->SetParent( pWnd );
				(*i)->SetParent( pParent );
			}
		}
	}
}

IMPL_LINK( ScTPValidationValue, EditSetFocusHdl, Edit *, /*pEdit*/ )
{
	sal_uInt16  nPos=maLbAllow.GetSelectEntryPos();
	
	if ( nPos == SC_VALIDDLG_ALLOW_RANGE )
	{
		SetupRefDlg();
	}

	return 0;
}

IMPL_LINK( ScTPValidationValue, KillFocusHdl, Window *, pWnd )
{
	if(	pWnd == m_pRefEdit || pWnd == &m_btnRef )
		if( ScValidationDlg *pValidationDlg = GetValidationDlg() )
			if ( (pValidationDlg->IsActive() || pValidationDlg->IsChildFocus() ) && !pValidationDlg->IsRefInputting() )
				if( ( !m_pRefEdit || !m_pRefEdit->HasFocus()) && !m_btnRef.HasFocus() )
				{
					RemoveRefDlg();
				}

	return 0;
}
//-->Added by PengYunQuan for Validity Cell Range Picker

// ----------------------------------------------------------------------------

IMPL_LINK( ScTPValidationValue, SelectHdl, ListBox*, EMPTYARG )
{
    sal_uInt16 nLbPos = maLbAllow.GetSelectEntryPos();
    bool bEnable = (nLbPos != SC_VALIDDLG_ALLOW_ANY);
    bool bRange = (nLbPos == SC_VALIDDLG_ALLOW_RANGE);
    bool bList = (nLbPos == SC_VALIDDLG_ALLOW_LIST);

    maCbAllow.Enable( bEnable );   // Leerzellen
    maFtValue.Enable( bEnable );
    maLbValue.Enable( bEnable );
    maFtMin.Enable( bEnable );
    maEdMin.Enable( bEnable );
    maEdList.Enable( bEnable );
    maFtMax.Enable( bEnable );
    maEdMax.Enable( bEnable );

    bool bShowMax = false;
    if( bRange )
        maFtMin.SetText( maStrRange );
    else if( bList )
        maFtMin.SetText( maStrList );
    else
    {
        switch( maLbValue.GetSelectEntryPos() )
        {
            case SC_VALIDDLG_DATA_EQUAL:
            case SC_VALIDDLG_DATA_NOTEQUAL:     maFtMin.SetText( maStrValue );  break;

            case SC_VALIDDLG_DATA_LESS:
            case SC_VALIDDLG_DATA_EQLESS:       maFtMin.SetText( maStrMax );    break;

            case SC_VALIDDLG_DATA_BETWEEN:
            case SC_VALIDDLG_DATA_NOTBETWEEN:   bShowMax = true;    // fall through
            case SC_VALIDDLG_DATA_GREATER:
            case SC_VALIDDLG_DATA_EQGREATER:    maFtMin.SetText( maStrMin );    break;

            default:
                DBG_ERRORFILE( "ScTPValidationValue::SelectHdl - unknown condition mode" );
        }
    }

    maCbShow.Show( bRange || bList );
    maCbSort.Show( bRange || bList );
    maFtValue.Show( !bRange && !bList );
    maLbValue.Show( !bRange && !bList );
    maEdMin.Show( !bList );
    maEdList.Show( bList );
    maFtMax.Show( bShowMax );
    maEdMax.Show( bShowMax );
    maFtHint.Show( bRange );
	//<!--Added by PengYunQuan for Validity Cell Range Picker
	m_btnRef.Show( bRange );
	//-->Added by PengYunQuan for Validity Cell Range Picker
    return 0;
}

IMPL_LINK( ScTPValidationValue, CheckHdl, CheckBox*, EMPTYARG )
{
    maCbSort.Enable( maCbShow.IsChecked() );
    return 0;
}


//========================================================================
//========================================================================
// Input Help Page

ScTPValidationHelp::ScTPValidationHelp( Window*			pParent,
										  const SfxItemSet& rArgSet )

	:	SfxTabPage		( pParent,
						  ScResId( TP_VALIDATION_INPUTHELP ),
						  rArgSet ),
		aTsbHelp		( this, ScResId( TSB_HELP ) ),
        aFlContent      ( this, ScResId( FL_CONTENT ) ),
		aFtTitle		( this, ScResId( FT_TITLE ) ),
		aEdtTitle		( this, ScResId( EDT_TITLE ) ),
		aFtInputHelp	( this, ScResId( FT_INPUTHELP ) ),
		aEdInputHelp	( this, ScResId( EDT_INPUTHELP ) ),

        mrArgSet            ( rArgSet )
{
	Init();
	FreeResource();
}

// -----------------------------------------------------------------------

__EXPORT ScTPValidationHelp::~ScTPValidationHelp()
{
}

// -----------------------------------------------------------------------

void ScTPValidationHelp::Init()
{
	//aLb.SetSelectHdl( LINK( this, ScTPValidationHelp, SelectHdl ) );

	aTsbHelp.EnableTriState( sal_False );
}

//------------------------------------------------------------------------

sal_uInt16* __EXPORT ScTPValidationHelp::GetRanges()
{
	return pValueRanges;
}

// -----------------------------------------------------------------------

SfxTabPage* __EXPORT ScTPValidationHelp::Create( Window*	pParent,
										 const SfxItemSet&	rArgSet )
{
	return ( new ScTPValidationHelp( pParent, rArgSet ) );
}

// -----------------------------------------------------------------------

void __EXPORT ScTPValidationHelp::Reset( const SfxItemSet& rArgSet )
{
	const SfxPoolItem* pItem;

	if ( rArgSet.GetItemState( FID_VALID_SHOWHELP, sal_True, &pItem ) == SFX_ITEM_SET )
		aTsbHelp.SetState( ((const SfxBoolItem*)pItem)->GetValue() ? STATE_CHECK : STATE_NOCHECK );
	else
		aTsbHelp.SetState( STATE_NOCHECK );

	if ( rArgSet.GetItemState( FID_VALID_HELPTITLE, sal_True, &pItem ) == SFX_ITEM_SET )
		aEdtTitle.SetText( ((const SfxStringItem*)pItem)->GetValue() );
	else
		aEdtTitle.SetText( EMPTY_STRING );

	if ( rArgSet.GetItemState( FID_VALID_HELPTEXT, sal_True, &pItem ) == SFX_ITEM_SET )
		aEdInputHelp.SetText( ((const SfxStringItem*)pItem)->GetValue() );
	else
		aEdInputHelp.SetText( EMPTY_STRING );
}

// -----------------------------------------------------------------------

sal_Bool __EXPORT ScTPValidationHelp::FillItemSet( SfxItemSet& rArgSet )
{
	rArgSet.Put( SfxBoolItem( FID_VALID_SHOWHELP, aTsbHelp.GetState() == STATE_CHECK ) );
	rArgSet.Put( SfxStringItem( FID_VALID_HELPTITLE, aEdtTitle.GetText() ) );
	rArgSet.Put( SfxStringItem( FID_VALID_HELPTEXT, aEdInputHelp.GetText() ) );

	return sal_True;
}

//========================================================================
//========================================================================
// Error Alert Page

ScTPValidationError::ScTPValidationError( Window*			pParent,
										  const SfxItemSet& rArgSet )

	:	SfxTabPage		( pParent,
						  ScResId( TP_VALIDATION_ERROR ),
						  rArgSet ),
		aTsbShow		( this, ScResId( TSB_SHOW ) ),
        aFlContent      ( this, ScResId( FL_CONTENT ) ),
		aFtAction		( this, ScResId( FT_ACTION ) ),
		aLbAction		( this, ScResId( LB_ACTION ) ),
		aBtnSearch		( this, ScResId( BTN_SEARCH ) ),
		aFtTitle		( this, ScResId( FT_TITLE ) ),
		aEdtTitle		( this, ScResId( EDT_TITLE ) ),
		aFtError		( this, ScResId( FT_ERROR ) ),
		aEdError		( this, ScResId( EDT_ERROR ) ),

        mrArgSet            ( rArgSet )
{
	Init();
	FreeResource();
}

// -----------------------------------------------------------------------

__EXPORT ScTPValidationError::~ScTPValidationError()
{
}

// -----------------------------------------------------------------------

void ScTPValidationError::Init()
{
	aLbAction.SetSelectHdl( LINK( this, ScTPValidationError, SelectActionHdl ) );
	aBtnSearch.SetClickHdl( LINK( this, ScTPValidationError, ClickSearchHdl ) );

	aLbAction.SelectEntryPos( 0 );
	aTsbShow.EnableTriState( sal_False );

	SelectActionHdl( NULL );
}

//------------------------------------------------------------------------

sal_uInt16* __EXPORT ScTPValidationError::GetRanges()
{
	return pValueRanges;
}

// -----------------------------------------------------------------------

SfxTabPage* __EXPORT ScTPValidationError::Create( Window*	pParent,
										 const SfxItemSet&	rArgSet )
{
	return ( new ScTPValidationError( pParent, rArgSet ) );
}

// -----------------------------------------------------------------------

void __EXPORT ScTPValidationError::Reset( const SfxItemSet& rArgSet )
{
	const SfxPoolItem* pItem;

	if ( rArgSet.GetItemState( FID_VALID_SHOWERR, sal_True, &pItem ) == SFX_ITEM_SET )
		aTsbShow.SetState( ((const SfxBoolItem*)pItem)->GetValue() ? STATE_CHECK : STATE_NOCHECK );
	else
        aTsbShow.SetState( STATE_CHECK );   // #111720# check by default

	if ( rArgSet.GetItemState( FID_VALID_ERRSTYLE, sal_True, &pItem ) == SFX_ITEM_SET )
		aLbAction.SelectEntryPos( ((const SfxAllEnumItem*)pItem)->GetValue() );
	else
		aLbAction.SelectEntryPos( 0 );

	if ( rArgSet.GetItemState( FID_VALID_ERRTITLE, sal_True, &pItem ) == SFX_ITEM_SET )
		aEdtTitle.SetText( ((const SfxStringItem*)pItem)->GetValue() );
	else
		aEdtTitle.SetText( EMPTY_STRING );

	if ( rArgSet.GetItemState( FID_VALID_ERRTEXT, sal_True, &pItem ) == SFX_ITEM_SET )
		aEdError.SetText( ((const SfxStringItem*)pItem)->GetValue() );
	else
		aEdError.SetText( EMPTY_STRING );

	SelectActionHdl( NULL );
}

// -----------------------------------------------------------------------

sal_Bool __EXPORT ScTPValidationError::FillItemSet( SfxItemSet& rArgSet )
{
	rArgSet.Put( SfxBoolItem( FID_VALID_SHOWERR, aTsbShow.GetState() == STATE_CHECK ) );
	rArgSet.Put( SfxAllEnumItem( FID_VALID_ERRSTYLE, aLbAction.GetSelectEntryPos() ) );
	rArgSet.Put( SfxStringItem( FID_VALID_ERRTITLE, aEdtTitle.GetText() ) );
	rArgSet.Put( SfxStringItem( FID_VALID_ERRTEXT, aEdError.GetText() ) );

	return sal_True;
}

// -----------------------------------------------------------------------

IMPL_LINK( ScTPValidationError, SelectActionHdl, ListBox*, EMPTYARG )
{
	ScValidErrorStyle eStyle = (ScValidErrorStyle) aLbAction.GetSelectEntryPos();
	sal_Bool bMacro = ( eStyle == SC_VALERR_MACRO );

	aBtnSearch.Enable( bMacro );
	aFtError.Enable( !bMacro );
	aEdError.Enable( !bMacro );

	return( 0L );
}

// -----------------------------------------------------------------------

IMPL_LINK( ScTPValidationError, ClickSearchHdl, PushButton*, EMPTYARG )
{
	Window* pOld = Application::GetDefDialogParent();
	Application::SetDefDialogParent( this );

	// Use static SfxApplication method to bring up selector dialog for
	// choosing a script
	::rtl::OUString aScriptURL = SfxApplication::ChooseScript();

	Application::SetDefDialogParent( pOld );

	if ( aScriptURL != NULL && aScriptURL.getLength() != 0 )
	{
		aEdtTitle.SetText( aScriptURL );
	}

	return( 0L );
}

//<!--Added by PengYunQuan for Validity Cell Range Picker
bool ScValidationDlg::EnterRefStatus()
{
	ScTabViewShell *pTabViewShell = GetTabViewShell();

	if( !pTabViewShell ) return false;

	sal_uInt16 nId  = SLOTID;
	SfxViewFrame* pViewFrm = pTabViewShell->GetViewFrame();
	SfxChildWindow* pWnd = pViewFrm->GetChildWindow( nId );

	if ( pWnd && pWnd->GetWindow()!= this ) pWnd = NULL;

	SC_MOD()->SetRefDialog( nId, pWnd ? sal_False : sal_True );

	return true;
}

bool ScValidationDlg::LeaveRefStatus()
{
	ScTabViewShell *pTabViewShell = GetTabViewShell();

	if( !pTabViewShell ) return false;

	sal_uInt16 nId  = SLOTID;
	SfxViewFrame* pViewFrm = pTabViewShell->GetViewFrame();
	//SfxChildWindow* pWnd = pViewFrm->GetChildWindow( nId );
    if ( pViewFrm->GetChildWindow( nId ) )
	{
		DoClose( nId );
	}
	return true;
}

bool ScValidationDlg::SetupRefDlg()
{
	if ( m_bOwnRefHdlr ) return false;
	if( EnterRefMode() )
	{
		SetModal( sal_False );
		return  /*SetChkShell( GetDocShell() ),*/ m_bOwnRefHdlr = true && EnterRefStatus();
	}

	return false;
}

bool ScValidationDlg::RemoveRefDlg( sal_Bool bRestoreModal /* = sal_True */ )
{
	bool bVisLock = false;
	bool bFreeWindowLock = false;

	ScTabViewShell *pTabVwSh = GetTabViewShell();

	if( !pTabVwSh ) return false;

	if ( SfxChildWindow* pWnd = pTabVwSh->GetViewFrame()->GetChildWindow( SID_VALIDITY_REFERENCE ) )
	{
		bVisLock = static_cast<ScValidityRefChildWin*>(pWnd)->LockVisible( true );
		bFreeWindowLock = static_cast<ScValidityRefChildWin*>(pWnd)->LockFreeWindow( true );
	}

	if ( !m_bOwnRefHdlr ) return false;
	if( LeaveRefStatus() && LeaveRefMode() ) 
	{
		m_bOwnRefHdlr = false;

		if( bRestoreModal )
			SetModal( sal_True );		
	}

	if ( SfxChildWindow* pWnd = pTabVwSh->GetViewFrame()->GetChildWindow( SID_VALIDITY_REFERENCE ) )
	{
		static_cast<ScValidityRefChildWin*>(pWnd)->LockVisible( bVisLock );
		static_cast<ScValidityRefChildWin*>(pWnd)->LockFreeWindow( bFreeWindowLock );
	}

	return true;
}

//TYPEINIT1( ScTPValidationValue, SfxTabPage )

void ScTPValidationValue::ScRefButtonEx::Click()
{
	if( ScTPValidationValue *pParent = dynamic_cast< ScTPValidationValue*>( GetParent() ) )
		pParent->OnClick( this );
		
	ScRefButton::Click();
}

void ScTPValidationValue::OnClick( Button *pBtn )
{
	if( pBtn == &m_btnRef )
		SetupRefDlg();
}

sal_Bool ScValidationDlg::IsChildFocus()
{
	if ( const Window *pWin = Application::GetFocusWindow() )
		while( NULL != ( pWin = pWin->GetParent() ) )
			if( pWin == this )
				return sal_True;

	return sal_False;
}


bool	ScValidationDlg::IsAlive()
{
	return SC_MOD()->IsAliveRefDlg( SLOTID, this );
}
//-->Added by PengYunQuan for Validity Cell Range Picker
