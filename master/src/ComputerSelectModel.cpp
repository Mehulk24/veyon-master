/*
 * ComputerSelectModel.cpp - data model for computer selection
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

#include "ComputerSelectModel.h"
#include "VeyonCore.h"

#if defined(QT_TESTLIB_LIB) && QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
#include <QAbstractItemModelTester>
#endif

ComputerSelectModel::ComputerSelectModel( QAbstractItemModel* sourceModel, QObject* parent ) :
	QSortFilterProxyModel(parent)
{
#if defined(QT_TESTLIB_LIB) && QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	new QAbstractItemModelTester( this, QAbstractItemModelTester::FailureReportingMode::Warning, this );
#endif

	setSourceModel( sourceModel );
	setFilterCaseSensitivity( Qt::CaseInsensitive );
	setFilterKeyColumn( -1 ); // filter all columns instead of first one only
	sort( 0 );
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
	setRecursiveFilteringEnabled(true);
#else
	vWarning() << "Built with Qt < 5.10 – filtering computers/users will not work properly";
#endif
}



QVariant ComputerSelectModel::value( const QModelIndex& index, const QString& role ) const
{
	return data( index, roleNames().key( role.toUtf8() ) );
}
