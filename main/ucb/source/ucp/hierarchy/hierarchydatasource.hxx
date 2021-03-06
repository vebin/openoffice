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



#ifndef _HIERARCHYDATASOURCE_HXX
#define _HIERARCHYDATASOURCE_HXX

#include <osl/mutex.hxx>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <cppuhelper/weak.hxx>
#include <ucbhelper/macros.hxx>

namespace cppu { class OInterfaceContainerHelper; }

namespace hierarchy_ucp {

//=========================================================================

class HierarchyDataSource : public cppu::OWeakObject,
                            public com::sun::star::lang::XServiceInfo,
                            public com::sun::star::lang::XTypeProvider,
                            public com::sun::star::lang::XComponent,
                            public com::sun::star::lang::XMultiServiceFactory
{
    osl::Mutex m_aMutex;
    com::sun::star::uno::Reference<
        com::sun::star::lang::XMultiServiceFactory > m_xSMgr;
    com::sun::star::uno::Reference<
        com::sun::star::lang::XMultiServiceFactory > m_xConfigProvider;
    cppu::OInterfaceContainerHelper * m_pDisposeEventListeners;

public:
    HierarchyDataSource( const com::sun::star::uno::Reference<
                            com::sun::star::lang::XMultiServiceFactory > &
                                rxServiceMgr );
    virtual ~HierarchyDataSource();

	// XInterface
	XINTERFACE_DECL()

    // XServiceInfo
	XSERVICEINFO_DECL()

	// XTypeProvider
	XTYPEPROVIDER_DECL()

    // XComponent
    virtual void SAL_CALL
    dispose()
        throw ( com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL
    addEventListener( const com::sun::star::uno::Reference<
                        com::sun::star::lang::XEventListener > & xListener )
        throw ( com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL
    removeEventListener( const com::sun::star::uno::Reference<
                            com::sun::star::lang::XEventListener > & aListener )
        throw ( com::sun::star::uno::RuntimeException );

    // XMultiServiceFactory
    virtual com::sun::star::uno::Reference<
        com::sun::star::uno::XInterface > SAL_CALL
    createInstance( const rtl::OUString & aServiceSpecifier )
        throw ( com::sun::star::uno::Exception,
                com::sun::star::uno::RuntimeException );
    virtual com::sun::star::uno::Reference<
        com::sun::star::uno::XInterface > SAL_CALL
    createInstanceWithArguments( const rtl::OUString & ServiceSpecifier,
                                 const com::sun::star::uno::Sequence<
                                    com::sun::star::uno::Any > & Arguments )
        throw ( com::sun::star::uno::Exception,
                com::sun::star::uno::RuntimeException );
    virtual com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL
    getAvailableServiceNames()
        throw ( com::sun::star::uno::RuntimeException );

	// Non-Interface methods

private:
    com::sun::star::uno::Reference<
        com::sun::star::uno::XInterface > SAL_CALL
    createInstanceWithArguments( const rtl::OUString & ServiceSpecifier,
                                 const com::sun::star::uno::Sequence<
                                    com::sun::star::uno::Any > & Arguments,
                                 bool bCheckArgs )
        throw ( com::sun::star::uno::Exception,
                com::sun::star::uno::RuntimeException );

    com::sun::star::uno::Reference<
        com::sun::star::lang::XMultiServiceFactory >
    getConfigProvider();

    bool
    createConfigPath( const rtl::OUString & rInPath, rtl::OUString & rOutPath );
};

} // namespace hierarchy_ucp

#endif /* !_HIERARCHYDATASOURCE_HXX */
