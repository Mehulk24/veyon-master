/*
 * PersistentLogonCredentials.h - declaration of PersistentLogonCredentials class
 *
 * Copyright (c) 2019-2024 Tobias Junghans <tobydox@veyon.io>
 *
 * This file is part of Veyon - https://veyon.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#pragma once

#include "CryptoCore.h"

// clazy:exclude=copyable-polymorphic

class PersistentLogonCredentials
{
public:
	using Password = CryptoCore::SecureArray;

	static bool read( QString* username, Password* password );
	static bool write( const QString& username, const Password& password );
	static bool clear();

};