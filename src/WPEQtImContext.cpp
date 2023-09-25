/*
 * Copyright (C) 2021 David Rosca <nowrep@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "WPEQtImContext.h"
#include "WPEQtView.h"

#include <QRect>
#include <QInputMethod>
#include <QInputMethodEvent>
#include <QGuiApplication>

typedef struct {
    WPEQtView *view;
    bool enabled;
    QRect *cursorArea;
    QString *surroundinText;
    unsigned cursorIndex;
    unsigned selectionIndex;
    Qt::InputMethodHints hints;
} WPEQtImContextPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(WPEQtImContext, wpeqt_im_context, WEBKIT_TYPE_INPUT_METHOD_CONTEXT)

#define PRIV(obj) ((WPEQtImContextPrivate *) wpeqt_im_context_get_instance_private(WPEQT_IM_CONTEXT(obj)))

static void wpeqt_im_context_finalize(GObject *object)
{
    WPEQtImContextPrivate *priv = PRIV(object);

    delete priv->cursorArea;
    delete priv->surroundinText;

    G_OBJECT_CLASS(wpeqt_im_context_parent_class)->finalize(object);
}

static void wpeqt_im_context_notify_focus_in(WebKitInputMethodContext *context)
{
    WPEQtImContextPrivate *priv = PRIV(context);

    priv->enabled = true;

    qApp->inputMethod()->update(Qt::ImQueryInput | Qt::ImEnabled | Qt::ImHints);
    if (!qApp->inputMethod()->isVisible() && priv->view->hasActiveFocus())
        qApp->inputMethod()->setVisible(true);
}

static void wpeqt_im_context_notify_focus_out(WebKitInputMethodContext *context)
{
    WPEQtImContextPrivate *priv = PRIV(context);

    priv->enabled = false;

    qApp->inputMethod()->update(Qt::ImQueryInput | Qt::ImEnabled | Qt::ImHints);
    if (qApp->inputMethod()->isVisible() && priv->view->hasActiveFocus())
        qApp->inputMethod()->setVisible(false);
}

static void wpeqt_im_context_notify_cursor_area(WebKitInputMethodContext *context, int x, int y, int width, int height)
{
    WPEQtImContextPrivate *priv = PRIV(context);

    // XXX: There's no way to query scroll position (beside JS), and the x/y
    //      here are in content coordinates instead of in viewport coordinates.
    *priv->cursorArea = QRect(std::min(x, (int)priv->view->width()), std::min(y, (int)priv->view->height()), width, height);

    qApp->inputMethod()->update(Qt::ImQueryInput | Qt::ImEnabled | Qt::ImHints);
    if (!qApp->inputMethod()->isVisible() && priv->enabled && priv->view->hasActiveFocus())
        qApp->inputMethod()->setVisible(true);
}

static void wpeqt_im_context_notify_surrounding(WebKitInputMethodContext *context, const char *text, guint length, guint cursor_index, guint selection_index)
{
    WPEQtImContextPrivate *priv = PRIV(context);

    *priv->surroundinText = QString::fromUtf8(text, length);
    priv->cursorIndex = cursor_index;
    priv->selectionIndex = selection_index;

    qApp->inputMethod()->update(Qt::ImQueryInput | Qt::ImEnabled | Qt::ImHints);
    if (!qApp->inputMethod()->isVisible() && priv->enabled && priv->view->hasActiveFocus())
        qApp->inputMethod()->setVisible(true);
}

static void wpeqt_im_context_reset(WebKitInputMethodContext *context)
{
    WPEQtImContextPrivate *priv = PRIV(context);

    priv->enabled = false;
    *priv->cursorArea = QRect();
    *priv->surroundinText = QString();
    priv->cursorIndex = 0;
    priv->selectionIndex = 0;
    priv->hints = Qt::ImhNone;

    qApp->inputMethod()->update(Qt::ImQueryInput | Qt::ImEnabled | Qt::ImHints);
}

static void wpeqt_im_context_class_init(WPEQtImContextClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = wpeqt_im_context_finalize;

    WebKitInputMethodContextClass *im_context_class = WEBKIT_INPUT_METHOD_CONTEXT_CLASS (klass);
    im_context_class->notify_focus_in = wpeqt_im_context_notify_focus_in;
    im_context_class->notify_focus_out = wpeqt_im_context_notify_focus_out;
    im_context_class->notify_cursor_area = wpeqt_im_context_notify_cursor_area;
    im_context_class->notify_surrounding = wpeqt_im_context_notify_surrounding;
    im_context_class->reset = wpeqt_im_context_reset;
}

static void wpeqt_im_context_content_type_changed(WPEQtImContext *context)
{
    WPEQtImContextPrivate *priv = PRIV(context);

    WebKitInputMethodContext *wk_context = WEBKIT_INPUT_METHOD_CONTEXT(context);
    WebKitInputPurpose purpose = webkit_input_method_context_get_input_purpose(wk_context);
    WebKitInputHints hints = webkit_input_method_context_get_input_hints(wk_context);

    priv->hints = Qt::ImhNone;

    switch (purpose) {
    case WEBKIT_INPUT_PURPOSE_DIGITS:
        priv->hints = Qt::ImhDigitsOnly;
        break;
    case WEBKIT_INPUT_PURPOSE_NUMBER:
        priv->hints = Qt::ImhFormattedNumbersOnly;
        break;
    case WEBKIT_INPUT_PURPOSE_PHONE:
        priv->hints = Qt::ImhDialableCharactersOnly;
        break;
    case WEBKIT_INPUT_PURPOSE_URL:
        priv->hints = Qt::ImhUrlCharactersOnly;
        break;
    case WEBKIT_INPUT_PURPOSE_EMAIL:
        priv->hints = Qt::ImhEmailCharactersOnly;
        break;
    case WEBKIT_INPUT_PURPOSE_PASSWORD:
        priv->hints = Qt::ImhHiddenText | Qt::ImhSensitiveData;
        break;
    default:
        break;
    }

    // if (hints & WEBKIT_INPUT_HINT_NONE)
    // if (hints & WEBKIT_INPUT_HINT_SPELLCHECK)
    if (hints & WEBKIT_INPUT_HINT_LOWERCASE)
        priv->hints |= Qt::ImhPreferLowercase;
    if (hints & WEBKIT_INPUT_HINT_UPPERCASE_CHARS)
        priv->hints |= Qt::ImhPreferUppercase;
    if (!(hints & (WEBKIT_INPUT_HINT_UPPERCASE_WORDS | WEBKIT_INPUT_HINT_UPPERCASE_SENTENCES)))
        priv->hints |= Qt::ImhNoAutoUppercase;
    // if (hints & WEBKIT_INPUT_HINT_INHIBIT_OSK)

    qApp->inputMethod()->update(Qt::ImQueryInput | Qt::ImEnabled | Qt::ImHints);
}

static void wpeqt_im_context_init(WPEQtImContext *context)
{
    g_signal_connect_swapped(context, "notify::input-purpose", G_CALLBACK(wpeqt_im_context_content_type_changed), context);
    g_signal_connect_swapped(context, "notify::input-hints", G_CALLBACK(wpeqt_im_context_content_type_changed), context);
}

WebKitInputMethodContext *wpeqt_im_context_new(WPEQtView *view)
{
    WebKitInputMethodContext *context = WEBKIT_INPUT_METHOD_CONTEXT(g_object_new(WPEQT_TYPE_IM_CONTEXT, NULL));

    WPEQtImContextPrivate *priv = PRIV(context);
    priv->view = view;
    priv->cursorArea = new QRect;
    priv->surroundinText = new QString;

    return context;
}

void wpeqt_im_context_event(WPEQtImContext *context, QInputMethodEvent *event)
{
    if (!event->commitString().isEmpty())
        g_signal_emit_by_name(WEBKIT_INPUT_METHOD_CONTEXT(context), "committed", qPrintable(event->commitString()));
}

void wpeqt_im_context_query(WPEQtImContext *context, Qt::InputMethodQuery query, QVariant *out)
{
    WPEQtImContextPrivate *priv = PRIV(context);

    switch (query) {
    case Qt::ImEnabled:
        *out = QVariant(priv->enabled);
        break;
    case Qt::ImCursorRectangle:
        *out = QVariant(*priv->cursorArea);
        break;
    case Qt::ImCursorPosition:
        *out = QVariant(priv->cursorIndex);
        break;
    case Qt::ImAnchorPosition:
        *out = QVariant(priv->selectionIndex);
        break;
    case Qt::ImSurroundingText:
        *out = QVariant(*priv->surroundinText);
        break;
    case Qt::ImHints:
        *out = QVariant(priv->hints);
        break;
    default:
        *out = QVariant();
        break;
    }
}
