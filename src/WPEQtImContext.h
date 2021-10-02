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

#include <wpe/webkit.h>

#include <QVariant>

class WPEQtView;
class QInputMethodEvent;

G_BEGIN_DECLS

#define WPEQT_TYPE_IM_CONTEXT (wpeqt_im_context_get_type())

G_DECLARE_DERIVABLE_TYPE(WPEQtImContext, wpeqt_im_context, WPEQT, IM_CONTEXT, WebKitInputMethodContext)

struct _WPEQtImContextClass {
    WebKitInputMethodContextClass parent_class;
};

WebKitInputMethodContext *wpeqt_im_context_new(WPEQtView *view);

void wpeqt_im_context_event(WPEQtImContext *context, QInputMethodEvent *event);
void wpeqt_im_context_query(WPEQtImContext *context, Qt::InputMethodQuery query, QVariant *out);

G_END_DECLS
