#include "WebPage_template.h"

#include "../../ESPEasy_common.h"

void getWebPageTemplateDefault(const String& tmplName, String& tmpl)
{
  tmpl.reserve(576);
  const bool addJS   = true;
  const bool addMeta = true;

  if (tmplName == F("TmplAP"))
  {
    getWebPageTemplateDefaultHead(tmpl, !addMeta, !addJS);
    tmpl += F("<body>");

    #ifndef WEBPAGE_TEMPLATE_AP_HEADER
    tmpl += F("<header class='apheader'>"
              "<h1>Welcome to ESP Easy Mega AP</h1>");
    #else
    tmpl += F(WEBPAGE_TEMPLATE_AP_HEADER);
    #endif
    
    tmpl += F("</header>");
              
    getWebPageTemplateDefaultContentSection(tmpl);
    getWebPageTemplateDefaultFooter(tmpl);
  }
  else if (tmplName == F("TmplMsg"))
  {
    getWebPageTemplateDefaultHead(tmpl, !addMeta, !addJS);
    tmpl += F("<body>");
    getWebPageTemplateDefaultHeader(tmpl, F("{{name}}"), false);
    getWebPageTemplateDefaultContentSection(tmpl);
    getWebPageTemplateDefaultFooter(tmpl);
  }
  else if (tmplName == F("TmplDsh"))
  {
    getWebPageTemplateDefaultHead(tmpl, !addMeta, addJS);
    tmpl += F(
      "<body>"
      "{{content}}"
      "</body></html>"
      );
  }
  else // all other template names e.g. TmplStd
  {
    getWebPageTemplateDefaultHead(tmpl, addMeta, addJS);
    tmpl += F("<body class='bodymenu'>"
              "<span class='message' id='rbtmsg'></span>");
    getWebPageTemplateDefaultHeader(tmpl, F("{{name}} {{logo}}"), true);
    getWebPageTemplateDefaultContentSection(tmpl);
    getWebPageTemplateDefaultFooter(tmpl);
  }
}

void getWebPageTemplateDefaultHead(String& tmpl, bool addMeta, bool addJS) {
  tmpl += F("<!DOCTYPE html><html lang='en'>"
            "<head>"
            "<meta charset='utf-8'/>"
            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
            "<title>{{name}}</title>");

  if (addMeta) { tmpl += F("{{meta}}"); }

  if (addJS) { tmpl += F("{{js}}"); }

  tmpl += F("{{css}}"
            "</head>");
}

void getWebPageTemplateDefaultHeader(String& tmpl, const String& title, bool addMenu) {
  {
    String tmp;
  #ifndef WEBPAGE_TEMPLATE_DEFAULT_HEADER
    tmp = F("<header class='headermenu'><h1>ESP Easy Mega: {{title}}</h1><BR>");
  #else // ifndef WEBPAGE_TEMPLATE_DEFAULT_HEADER
    tmp = F(WEBPAGE_TEMPLATE_DEFAULT_HEADER);
  #endif // ifndef WEBPAGE_TEMPLATE_DEFAULT_HEADER

    tmp.replace(F("{{title}}"), title);
    tmpl += tmp;
  }

  if (addMenu) { tmpl += F("{{menu}}"); }
  tmpl += F("</header>");
}

void getWebPageTemplateDefaultContentSection(String& tmpl) {
  tmpl += F("<section>"
            "<span class='message error'>"
            "{{error}}"
            "</span>"
            "{{content}}"
            "</section>"
            );
}

void getWebPageTemplateDefaultFooter(String& tmpl) {
  #ifndef WEBPAGE_TEMPLATE_DEFAULT_FOOTER
  tmpl += F("<footer>"
            "<br>"
            "<h6>Powered by <a href='http://www.letscontrolit.com' style='font-size: 15px; text-decoration: none'>Let's Control It</a> community</h6>"
            "</footer>"
            "</body></html>"
            );
#else // ifndef WEBPAGE_TEMPLATE_DEFAULT_FOOTER
  tmpl += F(WEBPAGE_TEMPLATE_DEFAULT_FOOTER);
#endif // ifndef WEBPAGE_TEMPLATE_DEFAULT_FOOTER
}
