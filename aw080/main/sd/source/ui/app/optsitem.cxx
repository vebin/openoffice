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
#include "precompiled_sd.hxx"
#include <svx/svdmodel.hxx>
#include <sfx2/app.hxx>
#include <sfx2/sfx.hrc>
#ifndef _SV_SALBTYPE_HRC //autogen
#include <vcl/salbtype.hxx>
#endif
#include <unotools/syslocale.hxx>

#include "app.hxx"
#include "optsitem.hxx"
#include "cfgids.hxx"
#include "FrameView.hxx"

using namespace ::rtl;
using namespace ::utl;
using namespace ::com::sun::star::uno;

#define B2U(_def_aStr) (OUString::createFromAscii(_def_aStr))

template< class T > T getSafeValue( const Any& rAny )
{
	T value = T();
	bool bOk = (rAny >>= value);

	DBG_ASSERT( bOk, "SdOptionsItem, wrong type from configuration!" );
	(void)bOk;

	return value;
}

// -----------------
// - SdOptionsItem -
// -----------------

SdOptionsItem::SdOptionsItem( const SdOptionsGeneric& rParent, const OUString rSubTree ) :
	ConfigItem	( rSubTree ),
	mrParent	( rParent )
{
}

// -----------------------------------------------------------------------------

SdOptionsItem::~SdOptionsItem()
{
}

// -----------------------------------------------------------------------------

void SdOptionsItem::Commit()
{
	if( IsModified() )
		mrParent.Commit( *this );
};

void SdOptionsItem::Notify( const com::sun::star::uno::Sequence<rtl::OUString>& )
{}


// -----------------------------------------------------------------------------

Sequence< Any >	SdOptionsItem::GetProperties( const Sequence< OUString >& rNames )
{
	return ConfigItem::GetProperties( rNames );
}

// -----------------------------------------------------------------------------

sal_Bool SdOptionsItem::PutProperties( const Sequence< OUString >& rNames, const Sequence< Any>& rValues )
{
	return ConfigItem::PutProperties( rNames, rValues );
}

// -----------------------------------------------------------------------------

void SdOptionsItem::SetModified()
{
	ConfigItem::SetModified();
}

// --------------------
// - SdOptionsGeneric -
// --------------------

SdOptionsGeneric::SdOptionsGeneric( sal_uInt16 nConfigId, const OUString& rSubTree ) :
	maSubTree	( rSubTree ),
	mpCfgItem	( NULL ),
	mnConfigId	( nConfigId ),
	mbInit		( rSubTree.getLength() == 0 )
{
}

// -----------------------------------------------------------------------------

void SdOptionsGeneric::Init() const
{
	if( !mbInit )
	{
		SdOptionsGeneric* pThis	= const_cast<SdOptionsGeneric*>(this);

		if( !mpCfgItem )
			pThis->mpCfgItem = new SdOptionsItem( *this, maSubTree );

		const Sequence< OUString >	aNames( GetPropertyNames() );
		const Sequence< Any >		aValues = mpCfgItem->GetProperties( aNames );

		if( aNames.getLength() && ( aValues.getLength() == aNames.getLength() ) )
		{
			const Any* pValues = aValues.getConstArray();

			pThis->EnableModify( false );
			pThis->mbInit = pThis->ReadData( pValues );
			pThis->EnableModify( true );
		}
		else
			pThis->mbInit = true;
	}
}

// -----------------------------------------------------------------------------

SdOptionsGeneric::~SdOptionsGeneric()
{
	delete mpCfgItem;
	mpCfgItem = NULL;
}

// -----------------------------------------------------------------------------

void SdOptionsGeneric::Commit( SdOptionsItem& rCfgItem ) const
{
	const Sequence< OUString >	aNames( GetPropertyNames() );
	Sequence< Any >				aValues( aNames.getLength() );

	if( aNames.getLength() && ( aValues.getLength() == aNames.getLength() ) )
	{
		if( (const_cast<SdOptionsGeneric*>(this))->WriteData( aValues.getArray() ) )
			rCfgItem.PutProperties( aNames, aValues );
		else
		{
			DBG_ERROR( "PutProperties failed" );
		}
	}
}

// -----------------------------------------------------------------------------

Sequence< OUString > SdOptionsGeneric::GetPropertyNames() const
{
	sal_uLong			nCount;
	const char**	ppPropNames;

	GetPropNameArray( ppPropNames, nCount );

	Sequence< OUString > aNames( nCount );
	OUString*			 pNames = aNames.getArray();

	for( sal_uLong i = 0; i < nCount; i++ )
		pNames[ i ] = OUString::createFromAscii( ppPropNames[ i ] );

	return aNames;
}

// -----------------------------------------------------------------------------

void SdOptionsGeneric::Store()
{
	if( mpCfgItem )
		mpCfgItem->Commit();
}

// -----------------------------------------------------------------------------

bool SdOptionsGeneric::isMetricSystem()
{
	SvtSysLocale aSysLocale;
    MeasurementSystem eSys = aSysLocale.GetLocaleDataPtr()->getMeasurementSystemEnum();

	return ( eSys == MEASURE_METRIC );
}

/*************************************************************************
|*
|* SdOptionsLayout
|*
\************************************************************************/

SdOptionsLayout::SdOptionsLayout(  sal_uInt16 nConfigId, bool bUseConfig ) :
	SdOptionsGeneric( nConfigId, bUseConfig ?
					  ( ( SDCFG_DRAW == nConfigId ) ?
						B2U( "Office.Draw/Layout" ) :
						B2U( "Office.Impress/Layout" ) ) :
					  OUString() ),
	bRuler( true ),
	bMoveOutline( true ),
	bDragStripes( false ),
	bHandlesBezier( false ),
	bHelplines( true ),
	nMetric((sal_uInt16)(isMetricSystem() ? FUNIT_CM : FUNIT_INCH)),
	nDefTab( 1250 )  
{
	EnableModify( true );
}

// -----------------------------------------------------------------------------

bool SdOptionsLayout::operator==( const SdOptionsLayout& rOpt ) const
{
	return(	IsRulerVisible() == rOpt.IsRulerVisible() &&
			IsMoveOutline() == rOpt.IsMoveOutline() &&
			IsDragStripes() == rOpt.IsDragStripes() &&
			IsHandlesBezier() == rOpt.IsHandlesBezier() &&
			IsHelplines() == rOpt.IsHelplines() &&
			GetMetric() == rOpt.GetMetric() &&
			GetDefTab() == rOpt.GetDefTab() );
}

// -----------------------------------------------------------------------------

void SdOptionsLayout::GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const
{
	static const char* aPropNamesMetric[] =
	{
		"Display/Ruler",
		"Display/Bezier",
		"Display/Contour",
		"Display/Guide",
		"Display/Helpline",
		"Other/MeasureUnit/Metric",
		"Other/TabStop/Metric"
	};

	static const char* aPropNamesNonMetric[] =
	{
		"Display/Ruler",
		"Display/Bezier",
		"Display/Contour",
		"Display/Guide",
		"Display/Helpline",
		"Other/MeasureUnit/NonMetric",
		"Other/TabStop/NonMetric"
	};

	rCount = 7;

	if( isMetricSystem() )
		ppNames = aPropNamesMetric;
	else
		ppNames = aPropNamesNonMetric;
}

// -----------------------------------------------------------------------------

bool SdOptionsLayout::ReadData( const Any* pValues )
{
	if( pValues[0].hasValue() ) SetRulerVisible( *(sal_Bool*) pValues[ 0 ].getValue() );
	if( pValues[1].hasValue() ) SetHandlesBezier( *(sal_Bool*) pValues[ 1 ].getValue() );
	if( pValues[2].hasValue() ) SetMoveOutline( *(sal_Bool*) pValues[ 2 ].getValue() );
	if( pValues[3].hasValue() ) SetDragStripes( *(sal_Bool*) pValues[ 3 ].getValue() );
	if( pValues[4].hasValue() ) SetHelplines( *(sal_Bool*) pValues[ 4 ].getValue() );
	if( pValues[5].hasValue() ) SetMetric( (sal_uInt16) *(sal_Int32*) pValues[ 5 ].getValue() );
	if( pValues[6].hasValue() ) SetDefTab( (sal_uInt16) *(sal_Int32*) pValues[ 6 ].getValue() );

	return true;
}

// -----------------------------------------------------------------------------

bool SdOptionsLayout::WriteData( Any* pValues ) const
{
	pValues[ 0 ] <<= IsRulerVisible();
	pValues[ 1 ] <<= IsHandlesBezier();
	pValues[ 2 ] <<= IsMoveOutline();
	pValues[ 3 ] <<= IsDragStripes();
	pValues[ 4 ] <<= IsHelplines();
	pValues[ 5 ] <<= (sal_Int32) GetMetric();
	pValues[ 6 ] <<= (sal_Int32) GetDefTab();

	return true;
}

/*************************************************************************
|*
|* SdOptionsLayoutItem
|*
\************************************************************************/

SdOptionsLayoutItem::SdOptionsLayoutItem( sal_uInt16 _nWhich )
:	SfxPoolItem		( _nWhich )
,	maOptionsLayout	( 0, false )
{
}

// ----------------------------------------------------------------------

SdOptionsLayoutItem::SdOptionsLayoutItem( sal_uInt16 _nWhich, SdOptions* pOpts, ::sd::FrameView* pView )
:	SfxPoolItem		( _nWhich )
,	maOptionsLayout	( 0, false )
{
	if( pOpts )
	{
		maOptionsLayout.SetMetric( pOpts->GetMetric() );
		maOptionsLayout.SetDefTab( pOpts->GetDefTab() );
	}

	if( pView )
	{
		maOptionsLayout.SetRulerVisible( pView->HasRuler() );
		maOptionsLayout.SetMoveOutline( !pView->IsNoDragXorPolys() );
		maOptionsLayout.SetDragStripes( pView->IsDragStripes() );
		maOptionsLayout.SetHandlesBezier( pView->IsPlusHandlesAlwaysVisible() );
		maOptionsLayout.SetHelplines( pView->IsHlplVisible() );
	}
	else if( pOpts )
	{
		maOptionsLayout.SetRulerVisible( pOpts->IsRulerVisible() );
		maOptionsLayout.SetMoveOutline( pOpts->IsMoveOutline() );
		maOptionsLayout.SetDragStripes( pOpts->IsDragStripes() );
		maOptionsLayout.SetHandlesBezier( pOpts->IsHandlesBezier() );
		maOptionsLayout.SetHelplines( pOpts->IsHelplines() );
	}
}

// ----------------------------------------------------------------------

SfxPoolItem* SdOptionsLayoutItem::Clone( SfxItemPool* ) const
{
	return new SdOptionsLayoutItem( *this );
}


// ----------------------------------------------------------------------

int SdOptionsLayoutItem::operator==( const SfxPoolItem& rAttr ) const
{
	const bool bSameType = SfxPoolItem::operator==( rAttr );
	DBG_ASSERT( bSameType, "SdOptionsLayoutItem::operator==(), differen pool item type!" );
	return bSameType && ( maOptionsLayout == static_cast< const SdOptionsLayoutItem& >( rAttr ).maOptionsLayout );
}

// -----------------------------------------------------------------------

void SdOptionsLayoutItem::SetOptions( SdOptions* pOpts ) const
{
	if( pOpts )
	{
		pOpts->SetRulerVisible( maOptionsLayout.IsRulerVisible() );
		pOpts->SetMoveOutline( maOptionsLayout.IsMoveOutline() );
		pOpts->SetDragStripes( maOptionsLayout.IsDragStripes() );
		pOpts->SetHandlesBezier( maOptionsLayout.IsHandlesBezier() );
		pOpts->SetHelplines( maOptionsLayout.IsHelplines() );
		pOpts->SetMetric( maOptionsLayout.GetMetric() );
		pOpts->SetDefTab( maOptionsLayout.GetDefTab() );
	}
}

/*************************************************************************
|*
|* SdOptionsContents
|*
\************************************************************************/

SdOptionsContents::SdOptionsContents( sal_uInt16 nConfigId, bool bUseConfig ) :
	SdOptionsGeneric( nConfigId, bUseConfig ?
					  ( ( SDCFG_DRAW == nConfigId ) ?
						B2U( "Office.Draw/Content" ) :
						B2U( "Office.Impress/Content" ) ) :
					  OUString() )
{
	EnableModify( true );
}

// -----------------------------------------------------------------------------

bool SdOptionsContents::operator==(const SdOptionsContents&) const
{
    return true;
}

// -----------------------------------------------------------------------------

void SdOptionsContents::GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const
{
	static const char* aPropNames[] =
	{
		"Display/PicturePlaceholder",
		"Display/ContourMode",
		"Display/LineContour",
		"Display/TextPlaceholder"
	};

	rCount = 4;
	ppNames = aPropNames;
}

// -----------------------------------------------------------------------------

bool SdOptionsContents::ReadData(const Any*)
{
	return true;
}

// -----------------------------------------------------------------------------

bool SdOptionsContents::WriteData( Any* pValues ) const
{
	//#i80528# no draft anymore
	pValues[ 0 ] <<= (sal_Bool)false;
	pValues[ 1 ] <<= (sal_Bool)false;
	pValues[ 2 ] <<= (sal_Bool)false;
	pValues[ 3 ] <<= (sal_Bool)false;

	return true;
}

/*************************************************************************
|*
|* SdOptionsContentsItem
|*
\************************************************************************/

SdOptionsContentsItem::SdOptionsContentsItem(sal_uInt16 _nWhich, SdOptions*, ::sd::FrameView*)
:	SfxPoolItem			( _nWhich )
,	maOptionsContents	( 0, false )
{
}

// ----------------------------------------------------------------------

SfxPoolItem* SdOptionsContentsItem::Clone( SfxItemPool* ) const
{
	return new SdOptionsContentsItem( *this );
}

// ----------------------------------------------------------------------

int SdOptionsContentsItem::operator==( const SfxPoolItem& rAttr ) const
{
	const bool bSameType = SfxPoolItem::operator==(rAttr);
	DBG_ASSERT( bSameType, "SdOptionsContentsItem::operator==(), differen pool item type!" );
	return bSameType && ( maOptionsContents == static_cast<const SdOptionsContentsItem&>( rAttr ).maOptionsContents );
}

// -----------------------------------------------------------------------

void SdOptionsContentsItem::SetOptions(SdOptions*) const
{
}

/*************************************************************************
|*
|* SdOptionsMisc
|*
\************************************************************************/

SdOptionsMisc::SdOptionsMisc( sal_uInt16 nConfigId, bool bUseConfig ) :
	SdOptionsGeneric( nConfigId, bUseConfig ?
					  ( ( SDCFG_DRAW == nConfigId ) ?
						B2U( "Office.Draw/Misc" ) :
						B2U( "Office.Impress/Misc" ) ) :
					  OUString() ),
	// #97016#
	nDefaultObjectSizeWidth(8000),
	nDefaultObjectSizeHeight(5000),
	bStartWithTemplate( true ),
	bMarkedHitMovesAlways( true ),
	bMoveOnlyDragging( false ),
	bCrookNoContortion( false ),
	bQuickEdit( GetConfigId() != SDCFG_DRAW ),
	bMasterPageCache( true ),
	bDragWithCopy( false ),
	bPickThrough( true ),
	bBigHandles( false ),
	bDoubleClickTextEdit( true ),
	bClickChangeRotation( false ),
	bStartWithActualPage( false ),
	bSolidDragging( true ),
	bSolidMarkHdl( true ),
	bSummationOfParagraphs( false ),
	bShowUndoDeleteWarning( true ),
    bSlideshowRespectZOrder( true ),
    bShowComments( true ),
	bPreviewNewEffects( true ),
	bPreviewChangedEffects( false ),
	bPreviewTransitions( true ),
	mnDisplay( 0 ),
	mnPenColor( 0xff0000 ),
	mnPenWidth( 150.0 ),

	// The default for 6.1-and-above documents is to use printer-independent
    // formatting.
	mnPrinterIndependentLayout (1)
{
	EnableModify( true );
}

// -----------------------------------------------------------------------------

bool SdOptionsMisc::operator==( const SdOptionsMisc& rOpt ) const
{
	return(	IsStartWithTemplate() == rOpt.IsStartWithTemplate() &&
			IsMarkedHitMovesAlways() == rOpt.IsMarkedHitMovesAlways() &&
			IsMoveOnlyDragging() == rOpt.IsMoveOnlyDragging() &&
			IsCrookNoContortion() == rOpt.IsCrookNoContortion() &&
			IsQuickEdit() == rOpt.IsQuickEdit() &&
			IsMasterPagePaintCaching() == rOpt.IsMasterPagePaintCaching() &&
			IsDragWithCopy() == rOpt.IsDragWithCopy() &&
			IsPickThrough() == rOpt.IsPickThrough() &&
			IsBigHandles() == rOpt.IsBigHandles() &&
			IsDoubleClickTextEdit() == rOpt.IsDoubleClickTextEdit() &&
			IsClickChangeRotation() == rOpt.IsClickChangeRotation() &&
			IsStartWithActualPage() == rOpt.IsStartWithActualPage() &&
			IsSummationOfParagraphs() == rOpt.IsSummationOfParagraphs() &&
			IsSolidDragging() == rOpt.IsSolidDragging() &&
			IsSolidMarkHdl() == rOpt.IsSolidMarkHdl() &&
			// #90356#
			IsShowUndoDeleteWarning() == rOpt.IsShowUndoDeleteWarning() &&
			IsSlideshowRespectZOrder() == rOpt.IsSlideshowRespectZOrder() &&
			GetPrinterIndependentLayout() == rOpt.GetPrinterIndependentLayout() &&
			// #97016#
			GetDefaultObjectSizeWidth() == rOpt.GetDefaultObjectSizeWidth() &&
			GetDefaultObjectSizeHeight() == rOpt.GetDefaultObjectSizeHeight() &&

			IsPreviewNewEffects() == rOpt.IsPreviewNewEffects() &&
			IsPreviewChangedEffects() == rOpt.IsPreviewChangedEffects() &&
			IsPreviewTransitions() == rOpt.IsPreviewTransitions() &&
			GetDisplay() == rOpt.GetDisplay() &&
			IsShowComments() == rOpt.IsShowComments() &&
			GetPresentationPenColor() == rOpt.GetPresentationPenColor() &&
			GetPresentationPenWidth() == rOpt.GetPresentationPenWidth()
		);
}

// -----------------------------------------------------------------------------

void SdOptionsMisc::GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const
{
	static const char* aPropNames[] =
	{
		"ObjectMoveable",
		"NoDistort",
		"TextObject/QuickEditing",
		"BackgroundCache",
		"CopyWhileMoving",
		"TextObject/Selectable",
		"BigHandles",
		"DclickTextedit",
		"RotateClick",
		"Preview",
		"ModifyWithAttributes",
		"SimpleHandles",
		// #97016#
		"DefaultObjectSize/Width",
		"DefaultObjectSize/Height",

        "Compatibility/PrinterIndependentLayout",

		"ShowComments",

		// just for impress
		"NewDoc/AutoPilot",
		"Start/CurrentPage",
		"Compatibility/AddBetween",
		// #90356#
		"ShowUndoDeleteWarning",
        "SlideshowRespectZOrder",

		"PreviewNewEffects",
		"PreviewChangedEffects",
		"PreviewTransitions",

		"Display",

		"PenColor",
		"PenWidth"
	};

	rCount = ( ( GetConfigId() == SDCFG_IMPRESS ) ? 27 : 16 );
	ppNames = aPropNames;
}

// -----------------------------------------------------------------------------

bool SdOptionsMisc::ReadData( const Any* pValues )
{
	if( pValues[0].hasValue() ) SetMarkedHitMovesAlways( *(sal_Bool*) pValues[ 0 ].getValue() );
	if( pValues[1].hasValue() ) SetCrookNoContortion( *(sal_Bool*) pValues[ 1 ].getValue() );
	if( pValues[2].hasValue() ) SetQuickEdit( *(sal_Bool*)pValues[ 2 ].getValue() );
	if( pValues[3].hasValue() ) SetMasterPagePaintCaching( *(sal_Bool*) pValues[ 3 ].getValue() );
	if( pValues[4].hasValue() ) SetDragWithCopy( *(sal_Bool*) pValues[ 4 ].getValue() );
	if( pValues[5].hasValue() ) SetPickThrough( *(sal_Bool*) pValues[ 5 ].getValue() );
	if( pValues[6].hasValue() ) SetBigHandles( *(sal_Bool*) pValues[ 6 ].getValue() );
	if( pValues[7].hasValue() ) SetDoubleClickTextEdit( *(sal_Bool*) pValues[ 7 ].getValue() );
	if( pValues[8].hasValue() ) SetClickChangeRotation( *(sal_Bool*) pValues[ 8 ].getValue() );
    //	if( pValues[9].hasValue() ) SetPreviewQuality( FRound( *(double*) pValues[ 9 ].getValue() ) );
	if( pValues[10].hasValue() ) SetSolidDragging( *(sal_Bool*) pValues[ 10 ].getValue() );
	if( pValues[11].hasValue() ) SetSolidMarkHdl( *(sal_Bool*) pValues[ 11 ].getValue() );
	// #97016#
	if( pValues[12].hasValue() ) SetDefaultObjectSizeWidth( *(sal_uInt32*) pValues[ 12 ].getValue() );
	if( pValues[13].hasValue() ) SetDefaultObjectSizeHeight( *(sal_uInt32*) pValues[ 13 ].getValue() );
	if( pValues[14].hasValue() ) SetPrinterIndependentLayout( *(sal_uInt16*) pValues[ 14 ].getValue() );

    if( pValues[15].hasValue() )
		SetShowComments(  *(sal_Bool*) pValues[ 15 ].getValue() );

	// just for Impress
	if( GetConfigId() == SDCFG_IMPRESS )
	{
		if( pValues[16].hasValue() )
			SetStartWithTemplate( *(sal_Bool*) pValues[ 16 ].getValue() );
		if( pValues[17].hasValue() )
			SetStartWithActualPage( *(sal_Bool*) pValues[ 17 ].getValue() );
		if( pValues[18].hasValue() )
			SetSummationOfParagraphs( *(sal_Bool*) pValues[ 18 ].getValue() );
		// #90356#
		if( pValues[19].hasValue() ) 
			SetShowUndoDeleteWarning( *(sal_Bool*) pValues[ 19 ].getValue() );

		if( pValues[20].hasValue() )
            SetSlideshowRespectZOrder(*(sal_Bool*) pValues[ 20 ].getValue());

		if( pValues[21].hasValue() )
			SetPreviewNewEffects(*(sal_Bool*) pValues[ 21 ].getValue());

		if( pValues[22].hasValue() )
			SetPreviewChangedEffects(*(sal_Bool*) pValues[ 22 ].getValue());

		if( pValues[23].hasValue() )
			SetPreviewTransitions(*(sal_Bool*) pValues[ 23 ].getValue());

		if( pValues[24].hasValue() )
			SetDisplay(*(sal_Int32*) pValues[ 24 ].getValue());

		if( pValues[25].hasValue() )
			SetPresentationPenColor( getSafeValue< sal_Int32 >( pValues[ 25 ] ) );

		if( pValues[26].hasValue() )
			SetPresentationPenWidth( getSafeValue< double >( pValues[ 26 ] ) );
	}

	return true;
}

// -----------------------------------------------------------------------------

bool SdOptionsMisc::WriteData( Any* pValues ) const
{
	pValues[ 0 ] <<= IsMarkedHitMovesAlways();
	pValues[ 1 ] <<= IsCrookNoContortion();
	pValues[ 2 ] <<= IsQuickEdit();
	pValues[ 3 ] <<= IsMasterPagePaintCaching();
	pValues[ 4 ] <<= IsDragWithCopy();
	pValues[ 5 ] <<= IsPickThrough();
	pValues[ 6 ] <<= IsBigHandles();
	pValues[ 7 ] <<= IsDoubleClickTextEdit();
	pValues[ 8 ] <<= IsClickChangeRotation();
    // The preview is not supported anymore.  Use a dummy value.
	pValues[ 9 ] <<= (double)0;// GetPreviewQuality();
	pValues[ 10 ] <<= IsSolidDragging();
	pValues[ 11 ] <<= IsSolidMarkHdl();
	// #97016#
	pValues[ 12 ] <<= GetDefaultObjectSizeWidth();
	pValues[ 13 ] <<= GetDefaultObjectSizeHeight();
	pValues[ 14 ] <<= GetPrinterIndependentLayout();
    pValues[ 15 ] <<= (sal_Bool)IsShowComments();
    
	// just for Impress
	if( GetConfigId() == SDCFG_IMPRESS )
	{
		pValues[ 16 ] <<= IsStartWithTemplate();
		pValues[ 17 ] <<= IsStartWithActualPage();
		pValues[ 18 ] <<= IsSummationOfParagraphs();
		// #90356#
		pValues[ 19 ] <<= IsShowUndoDeleteWarning();
		pValues[ 20 ] <<= IsSlideshowRespectZOrder();

		pValues[ 21 ] <<= IsPreviewNewEffects();
		pValues[ 22 ] <<= IsPreviewChangedEffects();
		pValues[ 23 ] <<= IsPreviewTransitions();

		pValues[ 24 ] <<= GetDisplay();

		pValues[ 25 ] <<= GetPresentationPenColor();
		pValues[ 26 ] <<= GetPresentationPenWidth();
	}

	return true;
}

/*************************************************************************
|*
|* SdOptionsMiscItem
|*
\************************************************************************/

SdOptionsMiscItem::SdOptionsMiscItem( sal_uInt16 _nWhich )
:	SfxPoolItem		( _nWhich )
,	maOptionsMisc	( 0, false )
{
}

// ----------------------------------------------------------------------

SdOptionsMiscItem::SdOptionsMiscItem( sal_uInt16 _nWhich, SdOptions* pOpts, ::sd::FrameView* pView )
:	SfxPoolItem		( _nWhich )
,	maOptionsMisc	( 0, false )
{
	if( pOpts )
	{
		maOptionsMisc.SetStartWithTemplate( pOpts->IsStartWithTemplate() );
		maOptionsMisc.SetStartWithActualPage( pOpts->IsStartWithActualPage() );
		maOptionsMisc.SetSummationOfParagraphs( pOpts->IsSummationOfParagraphs() );
		// #90356#
		maOptionsMisc.SetShowUndoDeleteWarning( pOpts->IsShowUndoDeleteWarning() );
		maOptionsMisc.SetPrinterIndependentLayout( pOpts->GetPrinterIndependentLayout() );
		// #97016#
		maOptionsMisc.SetDefaultObjectSizeWidth( pOpts->GetDefaultObjectSizeWidth() );
		maOptionsMisc.SetDefaultObjectSizeHeight( pOpts->GetDefaultObjectSizeHeight() );

		maOptionsMisc.SetPreviewNewEffects(pOpts->IsPreviewNewEffects());
		maOptionsMisc.SetPreviewChangedEffects(pOpts->IsPreviewChangedEffects());
		maOptionsMisc.SetPreviewTransitions(pOpts->IsPreviewTransitions());

		maOptionsMisc.SetDisplay(pOpts->GetDisplay());
		maOptionsMisc.SetShowComments( pOpts->IsShowComments() );

		maOptionsMisc.SetPresentationPenColor(pOpts->GetPresentationPenColor() );
		maOptionsMisc.SetPresentationPenWidth(pOpts->GetPresentationPenWidth() );
	}

	if( pView )
	{
		maOptionsMisc.SetMarkedHitMovesAlways( pView->IsMarkedHitMovesAlways() );
		maOptionsMisc.SetMoveOnlyDragging( pView->IsMoveOnlyDragging() );
		maOptionsMisc.SetCrookNoContortion( pView->IsCrookNoContortion() );
		maOptionsMisc.SetQuickEdit( pView->IsQuickEdit() );

		// #i26631#
		maOptionsMisc.SetMasterPagePaintCaching( pView->IsMasterPagePaintCaching() );

		maOptionsMisc.SetDragWithCopy( pView->IsDragWithCopy() );
		maOptionsMisc.SetPickThrough( pView->getSdrModelFromSdrView().IsPickThroughTransparentTextFrames() );
		maOptionsMisc.SetBigHandles( pView->IsBigHandles() );
		maOptionsMisc.SetDoubleClickTextEdit( pView->IsDoubleClickTextEdit() );
		maOptionsMisc.SetClickChangeRotation( pView->IsClickChangeRotation() );
		maOptionsMisc.SetSolidDragging( pView->IsSolidDragging() );
		maOptionsMisc.SetSolidMarkHdl( pView->IsSolidMarkHdl() );
	}
	else if( pOpts )
	{
		maOptionsMisc.SetMarkedHitMovesAlways( pOpts->IsMarkedHitMovesAlways() );
		maOptionsMisc.SetMoveOnlyDragging( pOpts->IsMoveOnlyDragging() );
		maOptionsMisc.SetCrookNoContortion( pOpts->IsCrookNoContortion() );
		maOptionsMisc.SetQuickEdit( pOpts->IsQuickEdit() );
		maOptionsMisc.SetMasterPagePaintCaching( pOpts->IsMasterPagePaintCaching() );
		maOptionsMisc.SetDragWithCopy( pOpts->IsDragWithCopy() );
		maOptionsMisc.SetPickThrough( pOpts->IsPickThrough() );
		maOptionsMisc.SetBigHandles( pOpts->IsBigHandles() );
		maOptionsMisc.SetDoubleClickTextEdit( pOpts->IsDoubleClickTextEdit() );
		maOptionsMisc.SetClickChangeRotation( pOpts->IsClickChangeRotation() );
		maOptionsMisc.SetSolidDragging( pOpts->IsSolidDragging() );
		maOptionsMisc.SetSolidMarkHdl( pOpts->IsSolidMarkHdl() );
	}
}

// ----------------------------------------------------------------------

SfxPoolItem* SdOptionsMiscItem::Clone( SfxItemPool* ) const
{
	return new SdOptionsMiscItem( *this );
}


// ----------------------------------------------------------------------

int SdOptionsMiscItem::operator==( const SfxPoolItem& rAttr ) const
{
	const bool bSameType = SfxPoolItem::operator==(rAttr);
	DBG_ASSERT( bSameType, "SdOptionsMiscItem::operator==(), differen pool item type!" );
	return bSameType && ( maOptionsMisc == static_cast< const SdOptionsMiscItem& >(rAttr).maOptionsMisc );
}

// -----------------------------------------------------------------------

void SdOptionsMiscItem::SetOptions( SdOptions* pOpts ) const
{
	if( pOpts )
	{
		pOpts->SetStartWithTemplate( maOptionsMisc.IsStartWithTemplate() );
		pOpts->SetMarkedHitMovesAlways( maOptionsMisc.IsMarkedHitMovesAlways() );
		pOpts->SetMoveOnlyDragging( maOptionsMisc.IsMoveOnlyDragging() );
		pOpts->SetCrookNoContortion( maOptionsMisc.IsCrookNoContortion() );
		pOpts->SetQuickEdit( maOptionsMisc.IsQuickEdit() );
		pOpts->SetMasterPagePaintCaching( maOptionsMisc.IsMasterPagePaintCaching() );
		pOpts->SetDragWithCopy( maOptionsMisc.IsDragWithCopy() );
		pOpts->SetPickThrough( maOptionsMisc.IsPickThrough() );
		pOpts->SetBigHandles( maOptionsMisc.IsBigHandles() );
		pOpts->SetDoubleClickTextEdit( maOptionsMisc.IsDoubleClickTextEdit() );
		pOpts->SetClickChangeRotation( maOptionsMisc.IsClickChangeRotation() );
		pOpts->SetStartWithActualPage( maOptionsMisc.IsStartWithActualPage() );
		pOpts->SetSummationOfParagraphs( maOptionsMisc.IsSummationOfParagraphs() );
		pOpts->SetSolidDragging( maOptionsMisc.IsSolidDragging() );
		pOpts->SetSolidMarkHdl( maOptionsMisc.IsSolidMarkHdl() );
		// #90356#
		pOpts->SetShowUndoDeleteWarning( maOptionsMisc.IsShowUndoDeleteWarning() );
		pOpts->SetPrinterIndependentLayout( maOptionsMisc.GetPrinterIndependentLayout() );
		pOpts->SetShowComments( maOptionsMisc.IsShowComments() );
		// #97016#
		pOpts->SetDefaultObjectSizeWidth( maOptionsMisc.GetDefaultObjectSizeWidth() );
		pOpts->SetDefaultObjectSizeHeight( maOptionsMisc.GetDefaultObjectSizeHeight() );

		pOpts->SetPreviewNewEffects( maOptionsMisc.IsPreviewNewEffects() );
		pOpts->SetPreviewChangedEffects( maOptionsMisc.IsPreviewChangedEffects() );
		pOpts->SetPreviewTransitions( maOptionsMisc.IsPreviewTransitions() );

		pOpts->SetDisplay( maOptionsMisc.GetDisplay() );
		
		pOpts->SetPresentationPenColor( maOptionsMisc.GetPresentationPenColor() );
		pOpts->SetPresentationPenWidth( maOptionsMisc.GetPresentationPenWidth() );
	}
}

/*************************************************************************
|*
|* SdOptionsSnap
|*
\************************************************************************/

SdOptionsSnap::SdOptionsSnap( sal_uInt16 nConfigId, bool bUseConfig ) :
	SdOptionsGeneric( nConfigId, bUseConfig ? 
					  ( ( SDCFG_DRAW == nConfigId ) ? 
						B2U( "Office.Draw/Snap" ) : 
						B2U( "Office.Impress/Snap" ) ) :
					  OUString() ),
	bSnapHelplines( true ),
	bSnapBorder( true ),
	bSnapFrame( false ),
	bSnapPoints( false ),
	bOrtho( false ),
	bBigOrtho( true ),
	bRotate( false ),
	nSnapArea( 5 ),
	nAngle( 1500 ),
	nBezAngle( 1500 )

{
	EnableModify( true );
}

// -----------------------------------------------------------------------------

bool SdOptionsSnap::operator==( const SdOptionsSnap& rOpt ) const
{
	return(	IsSnapHelplines() == rOpt.IsSnapHelplines() &&
			IsSnapBorder() == rOpt.IsSnapBorder() &&
			IsSnapFrame() == rOpt.IsSnapFrame() &&
			IsSnapPoints() == rOpt.IsSnapPoints() &&
			IsOrtho() == rOpt.IsOrtho() &&
			IsBigOrtho() == rOpt.IsBigOrtho() &&
			IsRotate() == rOpt.IsRotate() &&
			GetSnapArea() == rOpt.GetSnapArea() &&
			GetAngle() == rOpt.GetAngle() &&
			GetEliminatePolyPointLimitAngle() == rOpt.GetEliminatePolyPointLimitAngle() );
}

// -----------------------------------------------------------------------------

void SdOptionsSnap::GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const
{
	static const char* aPropNames[] =
	{
		"Object/SnapLine",
		"Object/PageMargin",
		"Object/ObjectFrame",
		"Object/ObjectPoint",
		"Position/CreatingMoving",
		"Position/ExtendEdges",
		"Position/Rotating",
		"Object/Range",
		"Position/RotatingValue",
		"Position/PointReduction"
	};
	
	rCount = 10;
	ppNames = aPropNames;
}

// -----------------------------------------------------------------------------

bool SdOptionsSnap::ReadData( const Any* pValues )
{
	if( pValues[0].hasValue() ) SetSnapHelplines( *(sal_Bool*) pValues[ 0 ].getValue() ); 
	if( pValues[1].hasValue() ) SetSnapBorder( *(sal_Bool*)pValues[ 1 ].getValue() );  
	if( pValues[2].hasValue() ) SetSnapFrame( *(sal_Bool*) pValues[ 2 ].getValue() ); 
	if( pValues[3].hasValue() ) SetSnapPoints( *(sal_Bool*) pValues[ 3 ].getValue() ); 
	if( pValues[4].hasValue() ) SetOrtho( *(sal_Bool*) pValues[ 4 ].getValue() ); 
	if( pValues[5].hasValue() ) SetBigOrtho( *(sal_Bool*) pValues[ 5 ].getValue() ); 
	if( pValues[6].hasValue() ) SetRotate( *(sal_Bool*) pValues[ 6 ].getValue() ); 
	if( pValues[7].hasValue() ) SetSnapArea( (sal_Int16) *(sal_Int32*) pValues[ 7 ].getValue() ); 
	if( pValues[8].hasValue() ) SetAngle( (sal_Int16) *(sal_Int32*) pValues[ 8 ].getValue() ); 
	if( pValues[9].hasValue() ) SetEliminatePolyPointLimitAngle( (sal_Int16) *(sal_Int32*) pValues[ 9 ].getValue() ); 

	return true;
}

// -----------------------------------------------------------------------------

bool SdOptionsSnap::WriteData( Any* pValues ) const
{
	pValues[ 0 ] <<= IsSnapHelplines(); 
	pValues[ 1 ] <<= IsSnapBorder(); 
	pValues[ 2 ] <<= IsSnapFrame(); 
	pValues[ 3 ] <<= IsSnapPoints(); 
	pValues[ 4 ] <<= IsOrtho(); 
	pValues[ 5 ] <<= IsBigOrtho(); 
	pValues[ 6 ] <<= IsRotate(); 
	pValues[ 7 ] <<= (sal_Int32) GetSnapArea(); 
	pValues[ 8 ] <<= (sal_Int32) GetAngle(); 
	pValues[ 9 ] <<= (sal_Int32) GetEliminatePolyPointLimitAngle(); 

	return true;
}

/*************************************************************************
|*
|* SdOptionsSnapItem
|*
\************************************************************************/

SdOptionsSnapItem::SdOptionsSnapItem( sal_uInt16 _nWhich )
:	SfxPoolItem		( _nWhich )
,	maOptionsSnap	( 0, false )
{
}

// ----------------------------------------------------------------------

SdOptionsSnapItem::SdOptionsSnapItem( sal_uInt16 _nWhich, SdOptions* pOpts, ::sd::FrameView* pView )
:	SfxPoolItem		( _nWhich )
,	maOptionsSnap	( 0, false )
{
	if( pView )
	{
		maOptionsSnap.SetSnapHelplines( pView->IsHelplineSnap() );
		maOptionsSnap.SetSnapBorder( pView->IsBorderSnap() );
		maOptionsSnap.SetSnapFrame( pView->IsOFrameSnap() );
		maOptionsSnap.SetSnapPoints( pView->IsOPointSnap() );
		maOptionsSnap.SetOrtho( pView->IsOrthogonal() );
		maOptionsSnap.SetBigOrtho( pView->IsBigOrthogonal() );
		maOptionsSnap.SetRotate( pView->IsAngleSnapEnabled() );
		maOptionsSnap.SetSnapArea( pView->GetDiscreteMagneticSnap() );
		maOptionsSnap.SetAngle( (sal_Int16) pView->GetSnapAngle() );
		maOptionsSnap.SetEliminatePolyPointLimitAngle( (sal_Int16) pView->GetEliminatePolyPointLimitAngle() );
	}
	else if( pOpts )
	{
		maOptionsSnap.SetSnapHelplines( pOpts->IsSnapHelplines() );
		maOptionsSnap.SetSnapBorder( pOpts->IsSnapBorder() );
		maOptionsSnap.SetSnapFrame( pOpts->IsSnapFrame() );
		maOptionsSnap.SetSnapPoints( pOpts->IsSnapPoints() );
		maOptionsSnap.SetOrtho( pOpts->IsOrtho() );
		maOptionsSnap.SetBigOrtho( pOpts->IsBigOrtho() );
		maOptionsSnap.SetRotate( pOpts->IsRotate() );
		maOptionsSnap.SetSnapArea( pOpts->GetSnapArea() );
		maOptionsSnap.SetAngle( pOpts->GetAngle() );
		maOptionsSnap.SetEliminatePolyPointLimitAngle( pOpts->GetEliminatePolyPointLimitAngle() );
	}
}

// ----------------------------------------------------------------------

SfxPoolItem* SdOptionsSnapItem::Clone( SfxItemPool* ) const
{
	return new SdOptionsSnapItem( *this );
}


// ----------------------------------------------------------------------

int SdOptionsSnapItem::operator==( const SfxPoolItem& rAttr ) const
{
	const bool bSameType = SfxPoolItem::operator==(rAttr);
	DBG_ASSERT( bSameType, "SdOptionsSnapItem::operator==(), differen pool item type!" );
	return bSameType && ( maOptionsSnap == static_cast< const SdOptionsSnapItem& >(rAttr).maOptionsSnap );
}

// -----------------------------------------------------------------------

void SdOptionsSnapItem::SetOptions( SdOptions* pOpts ) const
{
	if( pOpts )
	{
		pOpts->SetSnapHelplines( maOptionsSnap.IsSnapHelplines() );
		pOpts->SetSnapBorder( maOptionsSnap.IsSnapBorder() );
		pOpts->SetSnapFrame( maOptionsSnap.IsSnapFrame() );
		pOpts->SetSnapPoints( maOptionsSnap.IsSnapPoints() );
		pOpts->SetOrtho( maOptionsSnap.IsOrtho() );
		pOpts->SetBigOrtho( maOptionsSnap.IsBigOrtho() );
		pOpts->SetRotate( maOptionsSnap.IsRotate() );
		pOpts->SetSnapArea( maOptionsSnap.GetSnapArea() );
		pOpts->SetAngle( maOptionsSnap.GetAngle() );
		pOpts->SetEliminatePolyPointLimitAngle( maOptionsSnap.GetEliminatePolyPointLimitAngle() );
	}
}

/*************************************************************************
|*
|* SdOptionsZoom
|*
\************************************************************************/

SdOptionsZoom::SdOptionsZoom( sal_uInt16 nConfigId, bool bUseConfig ) :
	SdOptionsGeneric( nConfigId, ( bUseConfig &&  ( SDCFG_DRAW == nConfigId ) ) ? 
								 B2U( "Office.Draw/Zoom" ) : 
							     OUString() ),
	nX( 1 ),
    nY( 1 )
    
{
	EnableModify( true );
}

// -----------------------------------------------------------------------------

bool SdOptionsZoom::operator==( const SdOptionsZoom& rOpt ) const
{
	sal_Int32 nX1, nX2, nY1, nY2;

	GetScale( nX1, nY1 );
	rOpt.GetScale( nX2, nY2 );

	return( ( nX1 == nX2 ) && 
			( nY1 == nY2 ) );
}

// -----------------------------------------------------------------------------

void SdOptionsZoom::GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const
{
	static const char* aPropNames[] =
	{
		"ScaleX",
		"ScaleY"
	};
	
	rCount = ( GetConfigId() == SDCFG_DRAW ) ? 2 : 0;
	ppNames = aPropNames;
}

// -----------------------------------------------------------------------------

bool SdOptionsZoom::ReadData( const Any* pValues )
{
	sal_Int32 x = 1, y = 1;

	if( pValues[0].hasValue() ) x = ( *(sal_Int32*) pValues[ 0 ].getValue() ); 
	if( pValues[1].hasValue() ) y = ( *(sal_Int32*) pValues[ 1 ].getValue() );  
	
	SetScale( x, y );

	return true;
}

// -----------------------------------------------------------------------------

bool SdOptionsZoom::WriteData( Any* pValues ) const
{
	sal_Int32 x, y;

	GetScale( x, y );

	pValues[ 0 ] <<= (sal_Int32) x; 
	pValues[ 1 ] <<= (sal_Int32) y; 

	return true;
}

/*************************************************************************
|*
|* SdOptionsGrid
|*
\************************************************************************/

SdOptionsGrid::SdOptionsGrid( sal_uInt16 nConfigId, bool bUseConfig ) :
	SdOptionsGeneric( nConfigId, bUseConfig ? 
					  ( ( SDCFG_DRAW == nConfigId ) ? 
						B2U( "Office.Draw/Grid" ) : 
						B2U( "Office.Impress/Grid" ) ) :
					  OUString() )
{
	EnableModify( false );
	SetDefaults();
	EnableModify( true );
}

// -----------------------------------------------------------------------------

SdOptionsGrid::~SdOptionsGrid()
{
}

// -----------------------------------------------------------------------------

void SdOptionsGrid::SetDefaults()
{
	const sal_uInt32 nVal = 1000;

	SetFldDivisionX( nVal );
	SetFldDivisionY( nVal );
	SetFldDrawX( nVal );
	SetFldDrawY( nVal );
	SetFldSnapX( nVal );
	SetFldSnapY( nVal );
	SetUseGridSnap( false );
	SetSynchronize( true );
	SetGridVisible( false );
	SetEqualGrid( true );
}

// -----------------------------------------------------------------------------

bool SdOptionsGrid::operator==( const SdOptionsGrid& rOpt ) const
{
	return(	GetFldDrawX() == rOpt.GetFldDrawX() &&
			GetFldDivisionX() == rOpt.GetFldDivisionX() &&
			GetFldDrawY() == rOpt.GetFldDrawY() &&
			GetFldDivisionY() == rOpt.GetFldDivisionY() &&
			GetFldSnapX() == rOpt.GetFldSnapX() &&
			GetFldSnapY() == rOpt.GetFldSnapY() &&
			IsUseGridSnap() == rOpt.IsUseGridSnap() &&
			IsSynchronize() == rOpt.IsSynchronize() &&
			IsGridVisible() == rOpt.IsGridVisible() &&
			IsEqualGrid() == rOpt.IsEqualGrid() );
}

// -----------------------------------------------------------------------------

void SdOptionsGrid::GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const
{
	static const char* aPropNamesMetric[] =
	{
		"Resolution/XAxis/Metric",
		"Resolution/YAxis/Metric",
		"Subdivision/XAxis",
		"Subdivision/YAxis",
		"SnapGrid/XAxis/Metric",
		"SnapGrid/YAxis/Metric",
		"Option/SnapToGrid",
		"Option/Synchronize",
		"Option/VisibleGrid",
		"SnapGrid/Size"
	};

	static const char* aPropNamesNonMetric[] =
	{
		"Resolution/XAxis/NonMetric",
		"Resolution/YAxis/NonMetric",
		"Subdivision/XAxis",
		"Subdivision/YAxis",
		"SnapGrid/XAxis/NonMetric",
		"SnapGrid/YAxis/NonMetric",
		"Option/SnapToGrid",
		"Option/Synchronize",
		"Option/VisibleGrid",
		"SnapGrid/Size"
	};

	rCount = 10;

	if( isMetricSystem() )
		ppNames = aPropNamesMetric;
	else
		ppNames = aPropNamesNonMetric;
}

// -----------------------------------------------------------------------------

bool SdOptionsGrid::ReadData( const Any* pValues )
{
	if( pValues[0].hasValue() ) SetFldDrawX( *(sal_Int32*) pValues[ 0 ].getValue() ); 
	if( pValues[1].hasValue() ) SetFldDrawY( *(sal_Int32*) pValues[ 1 ].getValue() ); 

	if( pValues[2].hasValue() )
	{
		const sal_uInt32 nDivX = FRound( *(double*) pValues[ 2 ].getValue() );
		SetFldDivisionX( SvxOptionsGrid::GetFldDrawX() / ( nDivX + 1 ) ); 
	}

	if( pValues[3].hasValue() )
	{
		const sal_uInt32 nDivY = FRound( *(double*) pValues[ 3 ].getValue() );
		SetFldDivisionY( SvxOptionsGrid::GetFldDrawY() / ( nDivY + 1 ) ); 
	}

	if( pValues[4].hasValue() ) SetFldSnapX( *(sal_Int32*) pValues[ 4 ].getValue() ); 
	if( pValues[5].hasValue() ) SetFldSnapY( *(sal_Int32*) pValues[ 5 ].getValue() ); 
	if( pValues[6].hasValue() ) SetUseGridSnap( *(sal_Bool*) pValues[ 6 ].getValue() ); 
	if( pValues[7].hasValue() ) SetSynchronize( *(sal_Bool*) pValues[ 7 ].getValue() ); 
	if( pValues[8].hasValue() ) SetGridVisible( *(sal_Bool*) pValues[ 8 ].getValue() ); 
	if( pValues[9].hasValue() ) SetEqualGrid( *(sal_Bool*) pValues[ 9 ].getValue() ); 

	return true;
}

// -----------------------------------------------------------------------------

bool SdOptionsGrid::WriteData( Any* pValues ) const
{
	pValues[ 0 ] <<= (sal_Int32) GetFldDrawX(); 
	pValues[ 1 ] <<= (sal_Int32) GetFldDrawY(); 
	pValues[ 2 ] <<= ( GetFldDivisionX() ? ( (double) GetFldDrawX() / GetFldDivisionX() - 1.0 ) : (double) 0 );
	pValues[ 3 ] <<= ( GetFldDivisionY() ? ( (double) GetFldDrawY() / GetFldDivisionY() - 1.0 ) : (double) 0 );
	pValues[ 4 ] <<= (sal_Int32) GetFldSnapX(); 
	pValues[ 5 ] <<= (sal_Int32) GetFldSnapY(); 
	pValues[ 6 ] <<= IsUseGridSnap(); 
	pValues[ 7 ] <<= IsSynchronize(); 
	pValues[ 8 ] <<= IsGridVisible(); 
	pValues[ 9 ] <<= IsEqualGrid(); 

	return true;
}

/*************************************************************************
|*
|* SdOptionsGridItem
|*
\************************************************************************/

SdOptionsGridItem::SdOptionsGridItem( sal_uInt16 _nWhich, SdOptions* pOpts, ::sd::FrameView* pView ) :
	SvxGridItem( _nWhich )
{
	SetSynchronize( pOpts->IsSynchronize() );
	SetEqualGrid( pOpts->IsEqualGrid() );

	if( pView )
	{
		SetFldDrawX( pView->GetGridCoarse().Width() );
		SetFldDrawY( pView->GetGridCoarse().Height() );
		SetFldDivisionX( pView->GetGridFine().Width() ? ( GetFldDrawX() / pView->GetGridFine().Width() - 1 ) : 0 );
		SetFldDivisionY( pView->GetGridFine().Height() ? ( GetFldDrawY() / pView->GetGridFine().Height() - 1 ) : 0 );
		SetFldSnapX( basegfx::fround(pView->GetSnapGridWidthX()) );
		SetFldSnapY( basegfx::fround(pView->GetSnapGridWidthY()) );
		SetUseGridSnap( pView->IsGridSnap() );
		SetGridVisible( pView->IsGridVisible() );
	}
	else if(pOpts)
	{
		SetFldDrawX( pOpts->GetFldDrawX() );
		SetFldDrawY( pOpts->GetFldDrawY() );
		SetFldDivisionX( pOpts->GetFldDivisionX() ? ( pOpts->GetFldDrawX() / pOpts->GetFldDivisionX() - 1 ) : 0 );
		SetFldDivisionY( pOpts->GetFldDivisionY() ? ( pOpts->GetFldDrawY() / pOpts->GetFldDivisionY() - 1 ) : 0 );
		SetFldSnapX( pOpts->GetFldSnapX() );
		SetFldSnapY( pOpts->GetFldSnapY() );
		SetUseGridSnap( pOpts->IsUseGridSnap() );
		SetGridVisible( pOpts->IsGridVisible() );
	}
}

SfxPoolItem* SdOptionsGridItem::Clone(SfxItemPool* /*pPool*/) const
{
	return new SdOptionsGridItem( *this );
}

// -----------------------------------------------------------------------

void SdOptionsGridItem::SetOptions( SdOptions* pOpts ) const
{
	pOpts->SetFldDrawX( GetFldDrawX() );
	pOpts->SetFldDivisionX( GetFldDrawX() / ( GetFldDivisionX() + 1 ) );
	pOpts->SetFldDrawY( GetFldDrawY() );
	pOpts->SetFldDivisionY( GetFldDrawY() / ( GetFldDivisionY() + 1 ) );
	pOpts->SetFldSnapX( GetFldSnapX() );
	pOpts->SetFldSnapY( GetFldSnapY() );
	pOpts->SetUseGridSnap( GetUseGridSnap() );
	pOpts->SetSynchronize( GetSynchronize() );
	pOpts->SetGridVisible( GetGridVisible() );
	pOpts->SetEqualGrid( GetEqualGrid() );
}

/*************************************************************************
|*
|* SdOptionsPrint
|*
\************************************************************************/

SdOptionsPrint::SdOptionsPrint( sal_uInt16 nConfigId, bool bUseConfig ) :
	SdOptionsGeneric( nConfigId, bUseConfig ? 
					  ( ( SDCFG_DRAW == nConfigId ) ? 
						B2U( "Office.Draw/Print" ) : 
						B2U( "Office.Impress/Print" ) ) :
					  OUString() ),
	bDraw( true ),
	bNotes( false ),
	bHandout( false ),
	bOutline( false ),
	bDate( false ),
	bTime( false ),
	bPagename( false ),
	bHiddenPages( true ),
	bPagesize( false ),
	bPagetile( false ),
	bWarningPrinter( true ),
	bWarningSize( false ),
	bWarningOrientation( false ),
	bBooklet( false ),
	bFront( true ),
	bBack( true ),
	bCutPage( false ),
	bPaperbin( false ),
	mbHandoutHorizontal( true ),
	mnHandoutPages( 6 ),
	nQuality( 0 )
{
	EnableModify( true );
}

// -----------------------------------------------------------------------------

bool SdOptionsPrint::operator==( const SdOptionsPrint& rOpt ) const
{
	return( IsDraw() == rOpt.IsDraw() &&
			IsNotes() == rOpt.IsNotes() &&
			IsHandout() == rOpt.IsHandout() &&
			IsOutline() == rOpt.IsOutline() &&
			IsDate() == rOpt.IsDate() &&
			IsTime() == rOpt.IsTime() &&
			IsPagename() == rOpt.IsPagename() &&
			IsHiddenPages() == rOpt.IsHiddenPages() &&
			IsPagesize() == rOpt.IsPagesize() &&
			IsPagetile() == rOpt.IsPagetile() &&
			IsWarningPrinter() == rOpt.IsWarningPrinter() &&
			IsWarningSize() == rOpt.IsWarningSize() &&
			IsWarningOrientation() == rOpt.IsWarningOrientation() &&
			IsBooklet() == rOpt.IsBooklet() &&
			IsFrontPage() == rOpt.IsFrontPage() &&
			IsBackPage() == rOpt.IsBackPage() &&
			IsCutPage() == rOpt.IsCutPage() &&
			IsPaperbin() == rOpt.IsPaperbin() &&
			GetOutputQuality() == rOpt.GetOutputQuality() &&
			IsHandoutHorizontal() == rOpt.IsHandoutHorizontal() &&
			GetHandoutPages() == rOpt.GetHandoutPages() );
}

// -----------------------------------------------------------------------------

void SdOptionsPrint::GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const
{
	static const char* aDrawPropNames[] =
	{
		"Other/Date",
		"Other/Time",
		"Other/PageName",
		"Other/HiddenPage",
		"Page/PageSize",
		"Page/PageTile",
		// bWarningPrinter
		// bWarningSize
		// bWarningOrientation
		"Page/Booklet",
		"Page/BookletFront",
		"Page/BookletBack",
		// bCutPage
		"Other/FromPrinterSetup",
		"Other/Quality",
		"Content/Drawing",
	};
	static const char* aImpressPropNames[] =
	{
		"Other/Date",
		"Other/Time",
		"Other/PageName",
		"Other/HiddenPage",
		"Page/PageSize",
		"Page/PageTile",
		// bWarningPrinter
		// bWarningSize
		// bWarningOrientation
		"Page/Booklet",
		"Page/BookletFront",
		"Page/BookletBack",
		// bCutPage
		"Other/FromPrinterSetup",
		"Other/Quality",
		"Content/Presentation",
		"Content/Note",
		"Content/Handout",
		"Content/Outline",
		"Other/HandoutHorizontal",
		"Other/PagesPerHandout"
	};

	if( GetConfigId() == SDCFG_IMPRESS )
	{
		rCount = 17;
		ppNames = aImpressPropNames;
	}
	else
	{
		rCount = 12;
		ppNames = aDrawPropNames;
	}
}

// -----------------------------------------------------------------------------

bool SdOptionsPrint::ReadData( const Any* pValues )
{
	if( pValues[0].hasValue() ) SetDate( *(sal_Bool*) pValues[ 0 ].getValue() ); 
	if( pValues[1].hasValue() ) SetTime( *(sal_Bool*) pValues[ 1 ].getValue() ); 
	if( pValues[2].hasValue() ) SetPagename( *(sal_Bool*) pValues[ 2 ].getValue() ); 
	if( pValues[3].hasValue() ) SetHiddenPages( *(sal_Bool*) pValues[ 3 ].getValue() ); 
	if( pValues[4].hasValue() ) SetPagesize( *(sal_Bool*) pValues[ 4 ].getValue() ); 
	if( pValues[5].hasValue() ) SetPagetile( *(sal_Bool*) pValues[ 5 ].getValue() ); 
	if( pValues[6].hasValue() ) SetBooklet( *(sal_Bool*) pValues[ 6 ].getValue() ); 
	if( pValues[7].hasValue() ) SetFrontPage( *(sal_Bool*) pValues[ 7 ].getValue() ); 
	if( pValues[8].hasValue() ) SetBackPage( *(sal_Bool*) pValues[ 8 ].getValue() ); 
	if( pValues[9].hasValue() ) SetPaperbin( *(sal_Bool*) pValues[ 9 ].getValue() ); 
	if( pValues[10].hasValue() ) SetOutputQuality( (sal_uInt16) *(sal_Int32*) pValues[ 10 ].getValue() ); 
	if( pValues[11].hasValue() ) SetDraw( *(sal_Bool*) pValues[ 11 ].getValue() ); 

	// just for impress
	if( GetConfigId() == SDCFG_IMPRESS )
	{
		if( pValues[12].hasValue() ) SetNotes( *(sal_Bool*) pValues[ 12 ].getValue() ); 
		if( pValues[13].hasValue() ) SetHandout( *(sal_Bool*) pValues[ 13 ].getValue() ); 
		if( pValues[14].hasValue() ) SetOutline( *(sal_Bool*) pValues[ 14 ].getValue() ); 
		if( pValues[15].hasValue() ) SetHandoutHorizontal( *(sal_Bool*) pValues[15].getValue() );
		if( pValues[16].hasValue() ) SetHandoutPages( (sal_uInt16)*(sal_Int32*) pValues[16].getValue() );
	}

	return true;
}

// -----------------------------------------------------------------------------

bool SdOptionsPrint::WriteData( Any* pValues ) const
{
	pValues[ 0 ] <<= IsDate(); 
	pValues[ 1 ] <<= IsTime(); 
	pValues[ 2 ] <<= IsPagename();
	pValues[ 3 ] <<= IsHiddenPages();
	pValues[ 4 ] <<= IsPagesize(); 
	pValues[ 5 ] <<= IsPagetile();
	pValues[ 6 ] <<= IsBooklet();
	pValues[ 7 ] <<= IsFrontPage(); 
	pValues[ 8 ] <<= IsBackPage(); 
	pValues[ 9 ] <<= IsPaperbin();
	pValues[ 10 ] <<= (sal_Int32) GetOutputQuality(); 
	pValues[ 11 ] <<= IsDraw();
	
	// just for impress
	if( GetConfigId() == SDCFG_IMPRESS )
	{
		pValues[ 12 ] <<= IsNotes(); 
		pValues[ 13 ] <<= IsHandout(); 
		pValues[ 14 ] <<= IsOutline(); 
		pValues[ 15 ] <<= IsHandoutHorizontal();
		pValues[ 16 ] <<= GetHandoutPages();
	}

	return true;
}

/*************************************************************************
|*
|* SdOptionsPrintItem
|*
\************************************************************************/

SdOptionsPrintItem::SdOptionsPrintItem( sal_uInt16 _nWhich )
:	SfxPoolItem		( _nWhich )
,	maOptionsPrint	( 0, false )
{
}

// ----------------------------------------------------------------------

SdOptionsPrintItem::SdOptionsPrintItem( sal_uInt16 _nWhich, SdOptions* pOpts, ::sd::FrameView* )
:	SfxPoolItem		( _nWhich )
,	maOptionsPrint	( 0, false )
{
	if( pOpts )
	{
		maOptionsPrint.SetDraw( pOpts->IsDraw() );
		maOptionsPrint.SetNotes( pOpts->IsNotes() );
		maOptionsPrint.SetHandout( pOpts->IsHandout() );
		maOptionsPrint.SetOutline( pOpts->IsOutline() );
		maOptionsPrint.SetDate( pOpts->IsDate() );
		maOptionsPrint.SetTime( pOpts->IsTime() );
		maOptionsPrint.SetPagename( pOpts->IsPagename() );
		maOptionsPrint.SetHiddenPages( pOpts->IsHiddenPages() );
		maOptionsPrint.SetPagesize( pOpts->IsPagesize() );
		maOptionsPrint.SetPagetile( pOpts->IsPagetile() );
		maOptionsPrint.SetWarningPrinter( pOpts->IsWarningPrinter() );
		maOptionsPrint.SetWarningSize( pOpts->IsWarningSize() );
		maOptionsPrint.SetWarningOrientation( pOpts->IsWarningOrientation() );
		maOptionsPrint.SetBooklet( pOpts->IsBooklet() );
		maOptionsPrint.SetFrontPage( pOpts->IsFrontPage() );
		maOptionsPrint.SetBackPage( pOpts->IsBackPage() );
		maOptionsPrint.SetCutPage( pOpts->IsCutPage() );
		maOptionsPrint.SetPaperbin( pOpts->IsPaperbin() );
		maOptionsPrint.SetOutputQuality( pOpts->GetOutputQuality() );
	}
}

// ----------------------------------------------------------------------

SfxPoolItem* SdOptionsPrintItem::Clone( SfxItemPool* ) const
{
	return new SdOptionsPrintItem( *this );
}

// ----------------------------------------------------------------------

int SdOptionsPrintItem::operator==( const SfxPoolItem& rAttr ) const
{
	const bool bSameType = SfxPoolItem::operator==(rAttr);
	DBG_ASSERT( bSameType, "SdOptionsPrintItem::operator==(), differen pool item type!" );
	return bSameType && ( maOptionsPrint == static_cast< const SdOptionsPrintItem& >( rAttr ).maOptionsPrint );
}

// -----------------------------------------------------------------------

void SdOptionsPrintItem::SetOptions( SdOptions* pOpts ) const
{
	if( pOpts )
	{
		pOpts->SetDraw( maOptionsPrint.IsDraw() );
		pOpts->SetNotes( maOptionsPrint.IsNotes() );
		pOpts->SetHandout( maOptionsPrint.IsHandout() );
		pOpts->SetOutline( maOptionsPrint.IsOutline() );
		pOpts->SetDate( maOptionsPrint.IsDate() );
		pOpts->SetTime( maOptionsPrint.IsTime() );
		pOpts->SetPagename( maOptionsPrint.IsPagename() );
		pOpts->SetHiddenPages( maOptionsPrint.IsHiddenPages() );
		pOpts->SetPagesize( maOptionsPrint.IsPagesize() );
		pOpts->SetPagetile( maOptionsPrint.IsPagetile() );
		pOpts->SetWarningPrinter( maOptionsPrint.IsWarningPrinter() );
		pOpts->SetWarningSize( maOptionsPrint.IsWarningSize() );
		pOpts->SetWarningOrientation( maOptionsPrint.IsWarningOrientation() );
		pOpts->SetBooklet( maOptionsPrint.IsBooklet() );
		pOpts->SetFrontPage( maOptionsPrint.IsFrontPage() );
		pOpts->SetBackPage( maOptionsPrint.IsBackPage() );
		pOpts->SetCutPage( maOptionsPrint.IsCutPage() );
		pOpts->SetPaperbin( maOptionsPrint.IsPaperbin() );
		pOpts->SetOutputQuality( maOptionsPrint.GetOutputQuality() );
	}
}

/*************************************************************************
|*
|* SdOptions
|*
\************************************************************************/

SdOptions::SdOptions( sal_uInt16 nConfigId ) :
	SdOptionsLayout( nConfigId, true ),
	SdOptionsContents( nConfigId, true ),
	SdOptionsMisc( nConfigId, true ),
	SdOptionsSnap( nConfigId, true ),
	SdOptionsZoom( nConfigId, true ),
	SdOptionsGrid( nConfigId, true ),
	SdOptionsPrint( nConfigId, true )
{
}

// ----------------------------------------------------------------------

SdOptions::~SdOptions()
{
}

// ----------------------------------------------------------------------

void SdOptions::StoreConfig( sal_uLong nOptionsRange )
{
	if( nOptionsRange & SD_OPTIONS_LAYOUT )
		SdOptionsLayout::Store();

	if( nOptionsRange & SD_OPTIONS_CONTENTS )
		SdOptionsContents::Store();

	if( nOptionsRange & SD_OPTIONS_MISC )
		SdOptionsMisc::Store();

	if( nOptionsRange & SD_OPTIONS_SNAP )
		SdOptionsSnap::Store();

	if( nOptionsRange & SD_OPTIONS_ZOOM )
		SdOptionsZoom::Store();

	if( nOptionsRange & SD_OPTIONS_GRID )
		SdOptionsGrid::Store();

	if( nOptionsRange & SD_OPTIONS_PRINT )
		SdOptionsPrint::Store();
}
