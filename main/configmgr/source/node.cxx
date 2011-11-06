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



#include "precompiled_configmgr.hxx"
#include "sal/config.h"

#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "osl/diagnose.h"
#include "rtl/ref.hxx"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"

#include "data.hxx"
#include "node.hxx"
#include "nodemap.hxx"

namespace configmgr {

namespace {

namespace css = com::sun::star;

}

NodeMap & Node::getMembers() {
    OSL_ASSERT(false);
    throw css::uno::RuntimeException(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("this cannot happen")),
        css::uno::Reference< css::uno::XInterface >());
}

rtl::OUString Node::getTemplateName() const {
    return rtl::OUString();
}

void Node::setMandatory(int layer) {
    (void) layer; // avoid warnings
    OSL_ASSERT(layer == Data::NO_LAYER);
}

int Node::getMandatory() const {
    return Data::NO_LAYER;
}

void Node::setLayer(int layer) {
    OSL_ASSERT(layer >= layer_);
    layer_ = layer;
}

int Node::getLayer() const {
    return layer_;
}

void Node::setFinalized(int layer) {
    finalized_ = layer;
}

int Node::getFinalized() const {
    return finalized_;
}

rtl::Reference< Node > Node::getMember(rtl::OUString const & name) {
    NodeMap & members = getMembers();
    NodeMap::iterator i(members.find(name));
    return i == members.end() ? rtl::Reference< Node >() : i->second;
}

Node::Node(int layer): layer_(layer), finalized_(Data::NO_LAYER) {}

Node::Node(const Node & other):
    SimpleReferenceObject(), layer_(other.layer_), finalized_(other.finalized_)
{}

Node::~Node() {}

void Node::clear() {}

}
