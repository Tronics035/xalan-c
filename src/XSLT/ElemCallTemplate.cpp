/*
 * The Apache Software License, Version 1.1
 *
 *
 * Copyright (c) 1999 The Apache Software Foundation.  All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Xalan" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written 
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation and was
 * originally based on software copyright (c) 1999, International
 * Business Machines, Inc., http://www.ibm.com.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */
#include "ElemCallTemplate.hpp"



#include <sax/AttributeList.hpp>



#include <PlatformSupport/DOMStringHelper.hpp>



#include "AVT.hpp"
#include "Constants.hpp"
#include "ElemTemplate.hpp"
#include "Stylesheet.hpp"
#include "StylesheetConstructionContext.hpp"
#include "StylesheetExecutionContext.hpp"
#include "StylesheetRoot.hpp"



ElemCallTemplate::ElemCallTemplate(
			StylesheetConstructionContext&	constructionContext,
			Stylesheet&						stylesheetTree,
			const AttributeList&			atts,
			int								lineNumber,
			int								columnNumber) :
	ElemTemplateElement(constructionContext,
						stylesheetTree,
						lineNumber,
						columnNumber,
						Constants::ELEMNAME_CALLTEMPLATE),
	m_templateName()
{
	const unsigned int	nAttrs = atts.getLength();

	for(unsigned int i = 0; i < nAttrs; i++)
	{
		const XalanDOMChar*	const	aname = atts.getName(i);

		if(equals(aname, Constants::ATTRNAME_NAME))
		{
			m_templateName = QNameByValue(atts.getValue(i), getStylesheet().getNamespaces());        
		}
		else if(!isAttrOK(aname, atts, i, constructionContext))
		{
			constructionContext.error(Constants::ELEMNAME_CALLTEMPLATE_WITH_PREFIX_STRING + " has an illegal attribute: " + aname);
		}
	}

	if (m_templateName.isEmpty() == true)
	{
		constructionContext.error(Constants::ELEMNAME_CALLTEMPLATE_WITH_PREFIX_STRING + " requires a name attribute!");
	}
}


	
ElemCallTemplate::~ElemCallTemplate()
{
}



const XalanDOMString&
ElemCallTemplate::getElementName() const
{
	return Constants::ELEMNAME_CALLTEMPLATE_WITH_PREFIX_STRING;
}



void
ElemCallTemplate::execute(
			StylesheetExecutionContext&		executionContext,
			XalanNode*						sourceTree,
			XalanNode*						sourceNode,
			const QName&					mode) const
{
	ElemTemplateElement::execute(executionContext,	sourceTree, sourceNode, mode);

	assert(isEmpty(m_templateName.getLocalPart()) == false);

	const ElemTemplate* const	theTemplate =
			getStylesheet().getStylesheetRoot().findNamedTemplate(m_templateName,
					executionContext);

	if(0 != theTemplate)
	{
		StylesheetExecutionContext::ParamsPushPop	thePushPop(
				executionContext,
				theTemplate,
				*this,
				sourceTree,
				sourceNode,
				mode,
				theTemplate);

		theTemplate->execute(executionContext, sourceTree, sourceNode, mode);
	}
	else
	{
		executionContext.error("Could not find template named: '" +
				m_templateName.getLocalPart() + "'");
	}
}



bool
ElemCallTemplate::childTypeAllowed(int	xslToken) const
{
	bool	fResult = false;
	
	switch(xslToken)
	{
	// char-instructions 
	case Constants::ELEMNAME_WITHPARAM:
		fResult = true;
		break;
		
	default:
		break;
	}
	
	return fResult;
}
