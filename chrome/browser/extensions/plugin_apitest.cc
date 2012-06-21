// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/content_settings/host_content_settings_map.h"
#include "chrome/browser/extensions/extension_browsertest.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/prefs/pref_service.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/tab_contents/tab_contents.h"
#include "chrome/common/extensions/extension.h"
#include "chrome/common/pref_names.h"
#include "chrome/test/base/ui_test_utils.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/web_contents.h"
#include "net/base/net_util.h"

using content::NavigationController;
using content::WebContents;
using extensions::Extension;

// Tests that a renderer's plugin list is properly updated when we load and
// unload an extension that contains a plugin.
IN_PROC_BROWSER_TEST_F(ExtensionBrowserTest, PluginLoadUnload) {
  browser()->profile()->GetPrefs()->SetBoolean(prefs::kPluginsAlwaysAuthorize,
                                               true);

  FilePath extension_dir =
      test_data_dir_.AppendASCII("uitest").AppendASCII("plugins");

  ui_test_utils::NavigateToURL(browser(),
      net::FilePathToFileURL(extension_dir.AppendASCII("test.html")));
  WebContents* tab = browser()->GetActiveWebContents();

  // With no extensions, the plugin should not be loaded.
  bool result = false;
  ASSERT_TRUE(ui_test_utils::ExecuteJavaScriptAndExtractBool(
      tab->GetRenderViewHost(), L"", L"testPluginWorks()", &result));
  EXPECT_FALSE(result);

  ExtensionService* service = browser()->profile()->GetExtensionService();
  service->set_show_extensions_prompts(false);
  const size_t size_before = service->extensions()->size();
  const Extension* extension = LoadExtension(extension_dir);
  ASSERT_TRUE(extension);
  EXPECT_EQ(size_before + 1, service->extensions()->size());
  // Now the plugin should be in the cache.
  ASSERT_TRUE(ui_test_utils::ExecuteJavaScriptAndExtractBool(
      tab->GetRenderViewHost(), L"", L"testPluginWorks()", &result));
  // We don't allow extension plugins to run on ChromeOS.
#if defined(OS_CHROMEOS)
  EXPECT_FALSE(result);
#else
  EXPECT_TRUE(result);
#endif

  EXPECT_EQ(size_before + 1, service->extensions()->size());
  UnloadExtension(extension->id());
  EXPECT_EQ(size_before, service->extensions()->size());

  // Now the plugin should be unloaded, and the page should be broken.

  ASSERT_TRUE(ui_test_utils::ExecuteJavaScriptAndExtractBool(
      tab->GetRenderViewHost(), L"", L"testPluginWorks()", &result));
  EXPECT_FALSE(result);

  // If we reload the extension and page, it should work again.

  ASSERT_TRUE(LoadExtension(extension_dir));
  EXPECT_EQ(size_before + 1, service->extensions()->size());
  {
    ui_test_utils::WindowedNotificationObserver observer(
        content::NOTIFICATION_LOAD_STOP,
        content::Source<NavigationController>(
            &browser()->GetActiveWebContents()->GetController()));
    browser()->Reload(CURRENT_TAB);
    observer.Wait();
  }
  ASSERT_TRUE(ui_test_utils::ExecuteJavaScriptAndExtractBool(
      tab->GetRenderViewHost(), L"", L"testPluginWorks()", &result));
    // We don't allow extension plugins to run on ChromeOS.
  #if defined(OS_CHROMEOS)
    EXPECT_FALSE(result);
  #else
    EXPECT_TRUE(result);
  #endif
}

// Tests that private extension plugins are only visible to the extension.
IN_PROC_BROWSER_TEST_F(ExtensionBrowserTest, PluginPrivate) {
  browser()->profile()->GetPrefs()->SetBoolean(prefs::kPluginsAlwaysAuthorize,
                                               true);

  FilePath extension_dir =
      test_data_dir_.AppendASCII("uitest").AppendASCII("plugins_private");

  ExtensionService* service = browser()->profile()->GetExtensionService();
  service->set_show_extensions_prompts(false);
  const size_t size_before = service->extensions()->size();
  const Extension* extension = LoadExtension(extension_dir);
  ASSERT_TRUE(extension);
  EXPECT_EQ(size_before + 1, service->extensions()->size());

  // Load the test page through the extension URL, and the plugin should work.
  ui_test_utils::NavigateToURL(browser(),
      extension->GetResourceURL("test.html"));
  WebContents* tab = browser()->GetActiveWebContents();
  bool result = false;
  ASSERT_TRUE(ui_test_utils::ExecuteJavaScriptAndExtractBool(
      tab->GetRenderViewHost(), L"", L"testPluginWorks()", &result));
  // We don't allow extension plugins to run on ChromeOS.
#if defined(OS_CHROMEOS)
  EXPECT_FALSE(result);
#else
  // TODO(bauerb): This might wrongly fail if the plug-in takes too long
  // to load. Extension-private plug-ins don't appear in navigator.plugins, so
  // we can't check for the plug-in in Javascript.
  EXPECT_TRUE(result);
#endif

  // Regression test for http://crbug.com/131811: The plug-in should be
  // whitelisted for the extension (and only for the extension), so it should be
  // loaded even if content settings are set to block plug-ins.
  browser()->profile()->GetHostContentSettingsMap()->SetDefaultContentSetting(
      CONTENT_SETTINGS_TYPE_PLUGINS, CONTENT_SETTING_BLOCK);
  ASSERT_TRUE(ui_test_utils::ExecuteJavaScriptAndExtractBool(
      tab->GetRenderViewHost(), L"", L"testPluginWorks()", &result));
#if defined(OS_CHROMEOS)
  EXPECT_FALSE(result);
#else
  EXPECT_TRUE(result);
#endif

  // Now load it through a file URL. The plugin should not load.
  ui_test_utils::NavigateToURL(browser(),
      net::FilePathToFileURL(extension_dir.AppendASCII("test.html")));
  ASSERT_TRUE(ui_test_utils::ExecuteJavaScriptAndExtractBool(
      tab->GetRenderViewHost(), L"", L"testPluginWorks()", &result));
  EXPECT_FALSE(result);
}
