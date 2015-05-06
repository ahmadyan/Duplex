<<<<<<< Updated upstream
//
//  Configuration.cpp
//  duplex
//
//  Created by Adel Ahmadyan on 5/5/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
=======
>>>>>>> Stashed changes

#include "configuration.h"

Configuration::Configuration(string filename){
<<<<<<< Updated upstream
	cout << "Loading configuration" << endl;
	property_tree::read_xml(filename, pointerTree);

	// Traverse property tree example
	BOOST_FOREACH(property_tree::ptree::value_type const& node, pointerTree.get_child("purchaseOrder")){
		property_tree::ptree subtree = node.second;

		if (node.first == "item"){
			BOOST_FOREACH(property_tree::ptree::value_type const& v, subtree.get_child("")){
				string label = v.first;
				if (label != "<xmlattr>"){
					string value = subtree.get<string>(label);
					cout << label << ":  " << value << endl;
				}
			}
			cout << endl;
		}
	}

}

Configuration::~Configuration(){

}
=======
    property_tree::read_xml( filename, pointerTree  );
    
    // Traverse property tree example
    BOOST_FOREACH( property_tree::ptree::value_type const& node, pointerTree.get_child( "system.items" ) ){
        property_tree::ptree subtree = node.second;
        
        if( node.first == "item" ){
            BOOST_FOREACH( property_tree::ptree::value_type const& v, subtree.get_child( "" ) ){
                string label = v.first;
                if ( label != "<xmlattr>" ){
                    string value = subtree.get<string>( label );
                    cout << label << ":  " << value << endl;
                }
            }
            cout << endl;
        }
    }

}
>>>>>>> Stashed changes
