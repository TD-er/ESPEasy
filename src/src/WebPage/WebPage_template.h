#ifndef WEBPAGE_WEBPAGE_TEMPLATE_H
#define WEBPAGE_WEBPAGE_TEMPLATE_H

#include <Arduino.h>

void getWebPageTemplateDefault(const String& tmplName,
                               String      & tmpl);

void getWebPageTemplateDefaultHead(String& tmpl,
                                   bool    addMeta,
                                   bool    addJS);
void getWebPageTemplateDefaultHeader(String      & tmpl,
                                     const String& title,
                                     bool          addMenu);
void getWebPageTemplateDefaultContentSection(String& tmpl);
void getWebPageTemplateDefaultFooter(String& tmpl);


#endif // ifndef WEBPAGE_WEBPAGE_TEMPLATE_H
