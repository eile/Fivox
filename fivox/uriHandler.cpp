/* Copyright (c) 2014-2015, EPFL/Blue Brain Project
 *                          Stefan.Eilemann@epfl.ch
 *                          Jafet.VillafrancaDiaz@epfl.ch
 *
 * This file is part of Fivox <https://github.com/BlueBrain/Fivox>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "uriHandler.h"

#include <fivox/compartmentLoader.h>
#include <fivox/imageSource.h>
#include <fivox/somaLoader.h>
#include <fivox/spikeLoader.h>
#include <fivox/synapseLoader.h>
#include <fivox/vsdLoader.h>
#ifdef FIVOX_USE_BBPTESTDATA
#  include <BBP/TestDatasets.h>
#endif
#include <lunchbox/file.h>
#include <lunchbox/log.h>
#include <lunchbox/uri.h>
#include <boost/lexical_cast.hpp>

namespace fivox
{
namespace
{
using boost::lexical_cast;
const float _dt = 10.0f;
const size_t _maxBlockSize = LB_16MB;
const float _voxelsPerUM = 1.0f;
}

class URIHandler::Impl
{
public:
    explicit Impl( const std::string& parameters )
        : uri( parameters )
        , config( uri.getPath( ))
        , target( uri.getFragment( ))
#ifdef FIVOX_USE_BBPTESTDATA
        , useTestData( config.empty( ))
#else
        , useTestData( false )
#endif
    {}

    std::string getConfig() const
    {
        if( useTestData )
        {
            if( getType() == VSD )
                return lunchbox::getExecutablePath() +
                           "/../share/Fivox/configs/BlueConfigVSD";
#ifdef FIVOX_USE_BBPTESTDATA
            return BBP_TEST_BLUECONFIG;
#endif
        }
        return config;
    }

    std::string getTarget( const std::string& defaultTarget ) const
    {
        if( target.empty( ))
        {
            if( defaultTarget.empty() && useTestData )
            {
                if( getType() == SPIKES || getType() == SYNAPSES )
                    return "Column";
                return "Layer1";
            }
            return defaultTarget;
        }
        return target;
    }

    std::string getReport() const
    {
        const std::string& report( _get( "report" ));
        if( report.empty( ))
        {
            if( getType() == SOMAS )
                return useTestData ? "voltage" : "soma";
            return useTestData ? "allvoltage" : "voltage";
        }
        return report;
    }

    float getDt() const { return _get( "dt", _dt ); }
    std::string getSpikes() const { return _get( "spikes" ); }
    float getDuration() const { return _get( "duration",  getDt( )); }
    std::string getDyeCurve() const { return _get( "dyecurve" ); }
    float getResolution() const { return _get( "resolution", _voxelsPerUM); }
    size_t getMaxBlockSize() const
    {
        return _get( "maxBlockSize", _maxBlockSize );
    }

    VolumeType getType() const
    {
        const std::string& scheme = uri.getScheme();
        if( scheme == "fivoxsomas" )
            return SOMAS;
        if( scheme == "fivoxspikes" )
            return SPIKES;
        if( scheme == "fivoxsynapses" )
            return SYNAPSES;
        if( scheme == "fivoxvsd" )
            return VSD;
        if( scheme == "fivox" || scheme == "fivoxcompartments" )
            return COMPARTMENTS;
        return UNKNOWN;
    }

private:
    std::string _get( const std::string& param ) const
    {
        lunchbox::URI::ConstKVIter i = uri.findQuery( param );
        return i == uri.queryEnd() ? std::string() : i->second;
    }

    template< class T >
    T _get( const std::string& param, const T defaultValue ) const
    {
        const std::string& value = _get( param );
        if( value.empty( ))
            return defaultValue;

        try
        {
            return lexical_cast< T >( value );
        }
        catch( boost::bad_lexical_cast& )
        {
            LBWARN << "Invalid " << param << " specified, using "
                   << defaultValue << std::endl;
            return defaultValue;
        }
    }

    const lunchbox::URI uri;
    const std::string config;
    const std::string target;
    const bool useTestData;
};

URIHandler::URIHandler( const std::string& params )
    : _impl( new URIHandler::Impl( params ))
{}

URIHandler::~URIHandler()
{}

std::string URIHandler::getConfig() const
{
    return _impl->getConfig();
}

std::string URIHandler::getTarget( const std::string& defaultTarget ) const
{
    return _impl->getTarget( defaultTarget );
}

std::string URIHandler::getReport() const
{
    return _impl->getReport();
}

float URIHandler::getDt() const
{
    return _impl->getDt();
}

std::string URIHandler::getSpikes() const
{
    return _impl->getSpikes();
}

float URIHandler::getDuration() const
{
    return _impl->getDuration();
}

std::string URIHandler::getDyeCurve() const
{
    return _impl->getDyeCurve();
}

float URIHandler::getResolution() const
{
    return _impl->getResolution();

}
size_t URIHandler::getMaxBlockSize() const
{
    return _impl->getMaxBlockSize();
}

VolumeType URIHandler::getType() const
{
    return _impl->getType();
}

EventSourcePtr URIHandler::newLoader() const
{
    switch( getType( ))
    {
    case SOMAS:        return std::make_shared< SomaLoader >( *this );
    case SPIKES:       return std::make_shared< SpikeLoader >( *this );
    case SYNAPSES:     return std::make_shared< SynapseLoader >( *this );
    case VSD:          return std::make_shared< VSDLoader >( *this );
    case COMPARTMENTS: return std::make_shared< CompartmentLoader >( *this );
    default:           return nullptr;
    }
}

}