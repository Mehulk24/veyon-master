/*
 * BuiltinDirectoryConfigurationPage.cpp - implementation of BuiltinDirectoryConfigurationPage
 *
 * Copyright (c) 2017-2024 Tobias Junghans <tobydox@veyon.io>
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

#include <QJsonObject>

#include "BuiltinDirectoryConfiguration.h"
#include "BuiltinDirectoryConfigurationPage.h"
#include "Configuration/UiMapping.h"
#include "NetworkObjectModel.h"
#include "ObjectManager.h"

#include "ui_BuiltinDirectoryConfigurationPage.h"

BuiltinDirectoryConfigurationPage::BuiltinDirectoryConfigurationPage( BuiltinDirectoryConfiguration& configuration, QWidget* parent ) :
	ConfigurationPage( parent ),
	ui(new Ui::BuiltinDirectoryConfigurationPage),
	m_configuration( configuration )
{
	ui->setupUi(this);

	populateLocations();

	connect( ui->locationTableWidget, &QTableWidget::currentItemChanged,
			 this, &BuiltinDirectoryConfigurationPage::populateComputers );
}



BuiltinDirectoryConfigurationPage::~BuiltinDirectoryConfigurationPage()
{
	delete ui;
}



void BuiltinDirectoryConfigurationPage::resetWidgets()
{
	FOREACH_BUILTIN_DIRECTORY_CONFIG_PROPERTY(INIT_WIDGET_FROM_PROPERTY);

	populateLocations();

	ui->locationTableWidget->setCurrentCell( 0, 0 );
}



void BuiltinDirectoryConfigurationPage::connectWidgetsToProperties()
{
	FOREACH_BUILTIN_DIRECTORY_CONFIG_PROPERTY(CONNECT_WIDGET_TO_PROPERTY);
}



void BuiltinDirectoryConfigurationPage::applyConfiguration()
{
}



void BuiltinDirectoryConfigurationPage::addLocation()
{
	ObjectManager<NetworkObject> objectManager( m_configuration.networkObjects() );
	objectManager.add( NetworkObject( nullptr,
									  NetworkObject::Type::Location, tr( "New location" ),
									  {},
									  QUuid::createUuid() ) );
	m_configuration.setNetworkObjects( objectManager.objects() );

	populateLocations();

	ui->locationTableWidget->setCurrentCell( ui->locationTableWidget->rowCount()-1, 0 );
}



void BuiltinDirectoryConfigurationPage::updateLocation()
{
	auto currentLocationIndex = ui->locationTableWidget->currentIndex();
	if( currentLocationIndex.isValid() == false )
	{
		return;
	}

	ObjectManager<NetworkObject> objectManager( m_configuration.networkObjects() );
	objectManager.update( currentLocationObject() );
	m_configuration.setNetworkObjects( objectManager.objects() );

	populateLocations();

	ui->locationTableWidget->setCurrentIndex( currentLocationIndex );
}



void BuiltinDirectoryConfigurationPage::removeLocation()
{
	auto currentRow = ui->locationTableWidget->currentRow();

	ObjectManager<NetworkObject> objectManager( m_configuration.networkObjects() );
	objectManager.remove( currentLocationObject().uid(), true );
	m_configuration.setNetworkObjects( objectManager.objects() );

	populateLocations();

	if( currentRow > 0 )
	{
		ui->locationTableWidget->setCurrentCell( currentRow-1, 0 );
	}
	else if ( ui->locationTableWidget->rowCount() > 0 )
	{
		ui->locationTableWidget->setCurrentCell( currentRow, 0 );
	}
}



void BuiltinDirectoryConfigurationPage::addComputer()
{
	auto currentLocationUid = currentLocationObject().uid();
	if( currentLocationUid.isNull() )
	{
		return;
	}

	ObjectManager<NetworkObject> objectManager( m_configuration.networkObjects() );
	objectManager.add( NetworkObject( nullptr,
									  NetworkObject::Type::Host, tr( "New computer" ),
									  {},
									  QUuid::createUuid(),
									  currentLocationUid ) );
	m_configuration.setNetworkObjects( objectManager.objects() );

	populateComputers();

	ui->computerTableWidget->setCurrentCell( ui->computerTableWidget->rowCount()-1, 0 );
}



void BuiltinDirectoryConfigurationPage::updateComputer()
{
	auto currentComputerIndex = ui->computerTableWidget->currentIndex();
	if( currentComputerIndex.isValid() == false )
	{
		return;
	}

	ObjectManager<NetworkObject> objectManager( m_configuration.networkObjects() );
	objectManager.update( currentComputerObject() );
	m_configuration.setNetworkObjects( objectManager.objects() );

	populateComputers();

	ui->computerTableWidget->setCurrentIndex( currentComputerIndex );
}



void BuiltinDirectoryConfigurationPage::removeComputer()
{
	ObjectManager<NetworkObject> objectManager( m_configuration.networkObjects() );
	objectManager.remove( currentComputerObject().uid() );
	m_configuration.setNetworkObjects( objectManager.objects() );

	populateComputers();
}



void BuiltinDirectoryConfigurationPage::populateLocations()
{
	ui->locationTableWidget->setUpdatesEnabled( false );
	ui->locationTableWidget->clear();

	ui->addComputerButton->setEnabled( false );

	int rowCount = 0;

	const auto networkObjects = m_configuration.networkObjects();
	for( const auto& networkObjectValue : networkObjects )
	{
		const NetworkObject networkObject{networkObjectValue.toObject()};
		if( networkObject.type() == NetworkObject::Type::Location )
		{
			auto item = new QTableWidgetItem( networkObject.name() );
			item->setData( NetworkObjectModel::UidRole, networkObject.uid() );
			ui->locationTableWidget->setRowCount( ++rowCount );
			ui->locationTableWidget->setItem( rowCount-1, 0, item );
		}
	}

	ui->locationTableWidget->setUpdatesEnabled( true );

	if( rowCount > 0 )
	{
		ui->addComputerButton->setEnabled( true );
	}
}



void BuiltinDirectoryConfigurationPage::populateComputers()
{
	auto parentUid = currentLocationObject().uid();

	ui->computerTableWidget->setUpdatesEnabled( false );
	ui->computerTableWidget->setRowCount( 0 );

	int rowCount = 0;

	const auto networkObjects = m_configuration.networkObjects();
	for( const auto& networkObjectValue : networkObjects )
	{
		const NetworkObject networkObject{networkObjectValue.toObject()};

		if( networkObject.type() == NetworkObject::Type::Host &&
			networkObject.parentUid() == parentUid )
		{
			auto nameItem = new QTableWidgetItem( networkObject.name() );
			nameItem->setData( NetworkObjectModel::UidRole, networkObject.uid() );
			nameItem->setData( NetworkObjectModel::ParentUidRole, networkObject.parentUid() );

			ui->computerTableWidget->setRowCount( rowCount+1 );
			ui->computerTableWidget->setItem( rowCount, 0, nameItem );
			ui->computerTableWidget->setItem( rowCount, 1, new QTableWidgetItem( networkObject.property( NetworkObject::Property::HostAddress ).toString() ) );
			ui->computerTableWidget->setItem( rowCount, 2, new QTableWidgetItem( networkObject.property( NetworkObject::Property::MacAddress ).toString() ) );
			++rowCount;
		}
	}

	ui->computerTableWidget->setUpdatesEnabled( true );
}



NetworkObject BuiltinDirectoryConfigurationPage::currentLocationObject() const
{
	const auto selectedLocation = ui->locationTableWidget->currentItem();
	if( selectedLocation )
	{
		return NetworkObject( nullptr,
							  NetworkObject::Type::Location,
							  selectedLocation->text(),
							  {},
							  selectedLocation->data( NetworkObjectModel::UidRole ).toUuid(),
							  selectedLocation->data( NetworkObjectModel::ParentUidRole ).toUuid() );
	}

	return NetworkObject();
}



NetworkObject BuiltinDirectoryConfigurationPage::currentComputerObject() const
{
	const int row = ui->computerTableWidget->currentRow();
	if( row >= 0 )
	{
		auto nameItem = ui->computerTableWidget->item( row, 0 );
		auto hostAddressItem = ui->computerTableWidget->item( row, 1 );
		auto macAddressItem = ui->computerTableWidget->item( row, 2 );

		return NetworkObject( nullptr,
							  NetworkObject::Type::Host,
							  nameItem->text(),
							  {
								  { NetworkObject::propertyKey(NetworkObject::Property::HostAddress), hostAddressItem->text().trimmed() },
								  { NetworkObject::propertyKey(NetworkObject::Property::MacAddress), macAddressItem->text().trimmed() }
							  },
							  nameItem->data( NetworkObjectModel::UidRole ).toUuid(),
							  nameItem->data( NetworkObjectModel::ParentUidRole ).toUuid() );
	}

	return NetworkObject();
}
