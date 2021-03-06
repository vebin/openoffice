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



#ifndef _DBAUI_INDEXDIALOG_HXX_
#define _DBAUI_INDEXDIALOG_HXX_

#ifndef _SV_DIALOG_HXX
#include <vcl/dialog.hxx>
#endif
#ifndef _SV_FIXED_HXX
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_LSTBOX_HXX
#include <vcl/lstbox.hxx>
#endif
#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCONNECTION_HPP_
#include <com/sun/star/sdbc/XConnection.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _SV_TOOLBOX_HXX
#include <vcl/toolbox.hxx>
#endif
#ifndef _SVTREEBOX_HXX
#include <svtools/svtreebx.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_VIEWOPTIONS_HXX
#include <unotools/viewoptions.hxx>
#endif
#ifndef _DBAUI_INDEXES_HXX_
#include "indexes.hxx"
#endif
#ifndef DBAUI_TOOLBOXHELPER_HXX
#include "ToolBoxHelper.hxx"
#endif

//......................................................................
namespace dbaui
{
//......................................................................

	//==================================================================
	//= DbaIndexList
	//==================================================================
	class DbaIndexList : public SvTreeListBox
	{
	protected:
		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > m_xConnection;
		Link		m_aSelectHdl;
		Link		m_aEndEditHdl;
		sal_Bool	m_bSuspendSelectHdl;

	public:
		DbaIndexList(Window* _pParent, const ResId& _rId);

		void SetSelectHdl(const Link& _rHdl) { m_aSelectHdl = _rHdl; }
		Link GetSelectHdl() const { return m_aSelectHdl; }

		void SetEndEditHdl(const Link& _rHdl) { m_aEndEditHdl = _rHdl; }
		Link GetEndEditHdl() const { return m_aEndEditHdl; }

		virtual sal_Bool Select( SvLBoxEntry* pEntry, sal_Bool bSelect );

		void enableSelectHandler();
		void disableSelectHandler();

		void SelectNoHandlerCall( SvLBoxEntry* pEntry );

		inline void setConnection(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection)
		{
			 m_xConnection = _rxConnection;
		}

	protected:
		virtual sal_Bool EditedEntry( SvLBoxEntry* pEntry, const XubString& rNewText );

    private:
        using SvTreeListBox::Select;
	};

	//==================================================================
	//= DbaIndexDialog
	//==================================================================
	class IndexFieldsControl;
	class OIndexCollection;
	class DbaIndexDialog :	public ModalDialog,
							public OToolBoxHelper
	{
	protected:
		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > m_xConnection;
		SvtViewOptions			m_aGeometrySettings;

		ToolBox					m_aActions;
		DbaIndexList			m_aIndexes;
		FixedLine				m_aIndexDetails;
		FixedText				m_aDescriptionLabel;
		FixedText				m_aDescription;
		CheckBox				m_aUnique;
		FixedText				m_aFieldsLabel;
		IndexFieldsControl*		m_pFields;
		PushButton				m_aClose;
		HelpButton				m_aHelp;

		OIndexCollection*		m_pIndexes;
		SvLBoxEntry*			m_pPreviousSelection;
		sal_Bool				m_bEditAgain;

		::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
								m_xORB;
	public:
		DbaIndexDialog(
			Window* _pParent,
			const ::com::sun::star::uno::Sequence< ::rtl::OUString >& _rFieldNames,
			const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >& _rxIndexes,
			const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection,
			const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB,
			sal_Int32 _nMaxColumnsInIndex
			);
		virtual ~DbaIndexDialog();

		virtual void StateChanged( StateChangedType nStateChange );
		virtual void DataChanged( const DataChangedEvent& rDCEvt );

		/** will be called whenthe id of the image list is needed.
			@param	_eBitmapSet
				<svtools/imgdef.hxx>
			@param	_bHiContast
				<TRUE/> when in high contrast mode.
		*/
		virtual ImageList getImageList(sal_Int16 _eBitmapSet,sal_Bool _bHiContast) const;

		/** will be called when the controls need to be resized.
		*/
		virtual void resizeControls(const Size& _rDiff);

	protected:
		void fillIndexList();
		void updateToolbox();
		void updateControls(const SvLBoxEntry* _pEntry);

	protected:
		DECL_LINK( OnIndexSelected, DbaIndexList* );
		DECL_LINK( OnIndexAction, ToolBox* );
		DECL_LINK( OnEntryEdited, SvLBoxEntry* );
		DECL_LINK( OnModified, void* );
		DECL_LINK( OnCloseDialog, void* );

		DECL_LINK( OnEditIndexAgain, SvLBoxEntry* );

	private:
		void OnNewIndex();
		void OnDropIndex(sal_Bool _bConfirm = sal_True);
		void OnRenameIndex();
		void OnSaveIndex();
		void OnResetIndex();

		sal_Bool implCommit(SvLBoxEntry* _pEntry);
		sal_Bool implSaveModified(sal_Bool _bPlausibility = sal_True);
		sal_Bool implCommitPreviouslySelected();

		sal_Bool implDropIndex(SvLBoxEntry* _pEntry, sal_Bool _bRemoveFromCollection);

		sal_Bool implCheckPlausibility(const ConstIndexesIterator& _rPos);

		/** checks if the controls have to be replaced and moved.
		*/
		void checkControls();
	};

//......................................................................
}	// namespace dbaui
//......................................................................

#endif // _DBAUI_INDEXDIALOG_HXX_

