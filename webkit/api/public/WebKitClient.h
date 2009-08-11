/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebKitClient_h
#define WebKitClient_h

#include <time.h>

#include "WebCommon.h"
#include "WebLocalizedString.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

namespace WebKit {
    class WebClipboard;
    class WebData;
    class WebMessagePortChannel;
    class WebMimeRegistry;
    class WebPluginListBuilder;
    class WebSandboxSupport;
    class WebStorageNamespace;
    class WebString;
    class WebThemeEngine;
    class WebURL;
    class WebURLLoader;
    struct WebPluginInfo;
    template <typename T> class WebVector;

    class WebKitClient {
    public:
        // Must return non-null.
        virtual WebClipboard* clipboard() = 0;

        // Must return non-null.
        virtual WebMimeRegistry* mimeRegistry() = 0;

        // May return null if sandbox support is not necessary
        virtual WebSandboxSupport* sandboxSupport() = 0;

        // May return null on some platforms.
        virtual WebThemeEngine* themeEngine() = 0;


        // DOM Storage --------------------------------------------------

        // Return a LocalStorage namespace that corresponds to the following
        // path.
        virtual WebStorageNamespace* createLocalStorageNamespace(
            const WebString& path) = 0;

        // Return a new SessionStorage namespace.
        virtual WebStorageNamespace* createSessionStorageNamespace() = 0;


        // File ----------------------------------------------------------------

        // Various file/directory related functions.  These map 1:1 with
        // functions in WebCore's FileSystem.h.
        virtual bool fileExists(const WebString& path) = 0;
        virtual bool deleteFile(const WebString& path) = 0;
        virtual bool deleteEmptyDirectory(const WebString& path) = 0;
        virtual bool getFileSize(const WebString& path, long long& result) = 0;
        virtual bool getFileModificationTime(const WebString& path, time_t& result) = 0;
        virtual WebString directoryName(const WebString& path) = 0;
        virtual WebString pathByAppendingComponent(const WebString& path, const WebString& component) = 0;
        virtual bool makeAllDirectories(const WebString& path) = 0;


        // History -------------------------------------------------------------

        // Returns the hash for the given canonicalized URL for use in visited
        // link coloring.
        virtual unsigned long long visitedLinkHash(
            const char* canonicalURL, size_t length) = 0;

        // Returns whether the given link hash is in the user's history.  The
        // hash must have been generated by calling VisitedLinkHash().
        virtual bool isLinkVisited(unsigned long long linkHash) = 0;

        // HTML5 DB ------------------------------------------------------------

#if defined(OS_WIN)
        typedef HANDLE FileType;
#else
        typedef int FileType;
#endif

        // Opens a database file
        virtual FileType databaseOpenFile(const WebString& fileName, int desiredFlags) = 0;

        // Deletes a database file and returns the error code
        virtual bool databaseDeleteFile(const WebString& fileName) = 0;

        // Returns the attributes of the given database file
        virtual long databaseGetFileAttributes(const WebString& fileName) = 0;

        // Returns the size of the given database file
        virtual long long databaseGetFileSize(const WebString& fileName) = 0;


        // Message Ports -------------------------------------------------------

        // Creates a Message Port Channel.  This can be called on any thread.
        // The returned object should only be used on the thread it was created on.
        virtual WebMessagePortChannel* createMessagePortChannel() = 0;


        // Network -------------------------------------------------------------

        virtual void setCookies(
            const WebURL& url, const WebURL& policyURL, const WebString& cookies) = 0;
        virtual WebString cookies(const WebURL& url, const WebURL& policyURL) = 0;

        // A suggestion to prefetch IP information for the given hostname.
        virtual void prefetchHostName(const WebString&) = 0;

        // Returns a new WebURLLoader instance.
        virtual WebURLLoader* createURLLoader() = 0;


        // Plugins -------------------------------------------------------------

        // If refresh is true, then cached information should not be used to
        // satisfy this call.
        virtual void getPluginList(bool refresh, WebPluginListBuilder*) = 0;


        // Profiling -----------------------------------------------------------

        virtual void decrementStatsCounter(const char* name) = 0;
        virtual void incrementStatsCounter(const char* name) = 0;

        // An event is identified by the pair (name, id).  The extra parameter
        // specifies additional data to log with the event.
        virtual void traceEventBegin(const char* name, void* id, const char* extra) = 0;
        virtual void traceEventEnd(const char* name, void* id, const char* extra) = 0;


        // Resources -----------------------------------------------------------

        // Returns a blob of data corresponding to the named resource.
        virtual WebData loadResource(const char* name) = 0;

        // Returns a localized string resource (with an optional numeric
        // parameter value).
        virtual WebString queryLocalizedString(WebLocalizedString::Name) = 0;
        virtual WebString queryLocalizedString(WebLocalizedString::Name, int numericValue) = 0;


        // Sandbox ------------------------------------------------------------

        // In some browsers, a "sandbox" restricts what operations a program
        // is allowed to preform.  Such operations are typically abstracted out
        // via this API, but sometimes (like in HTML 5 database opening) WebKit
        // needs to behave differently based on whether it's restricted or not.
        // In these cases (and these cases only) you can call this function.
        // It's OK for this value to be conservitive (i.e. true even if the
        // sandbox isn't active).
        virtual bool sandboxEnabled() = 0;


        // Sudden Termination --------------------------------------------------

        // Disable/Enable sudden termination.
        virtual void suddenTerminationChanged(bool enabled) = 0;


        // System --------------------------------------------------------------

        // Returns a value such as "en-US".
        virtual WebString defaultLocale() = 0;

        // Wall clock time in seconds since the epoch.
        virtual double currentTime() = 0;

        // Delayed work is driven by a shared timer.
        virtual void setSharedTimerFiredFunction(void (*func)()) = 0;
        virtual void setSharedTimerFireTime(double fireTime) = 0;
        virtual void stopSharedTimer() = 0;

        // Callable from a background WebKit thread.
        virtual void callOnMainThread(void (*func)()) = 0;

    protected:
        ~WebKitClient() { }
    };

} // namespace WebKit

#endif
