
/* Copyright (c) 2014-2016, EPFL/Blue Brain Project
 *                          Stefan.Eilemann@epfl.ch
 *                          Jafet.VillafrancaDiaz@epfl.ch
 *                          Daniel.Nachbaur@epfl.ch
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

#ifndef FIVOX_URIHANDLER_H
#define FIVOX_URIHANDLER_H

#include <brion/types.h>
#include <fivox/types.h>
#include <memory>

namespace fivox
{

/**
 * Process an URI to provide all the parameters specified in it.
 *
 * The following parameters are parsed:
 * @snippet apps/commandLineApplication.h VolumeParameters
 */
class URIHandler
{
public:
    /**
     * Construct a new URI handler.
     *
     * @param parameters URI containing the parameters in the specified form
     */
    explicit URIHandler( const URI& parameters );
    virtual ~URIHandler(); //!< Destruct this parameter processor

    /**
     * @return the BlueConfig object from the parameters. For empty parameters,
     *         it returns the TestData's BlueConfig if available.
     */
    const brion::BlueConfig& getConfig() const;

    /**
     * @return the GIDs from the 'postTarget' parameter (if specified, by
     *         default not), or the 'target' parameter. If no target was
     *         specified, it returns the GIDs from the BlueConfig's circuit
     *         target. If this is empty (and TestData is available), it returns
     *         the GIDs from the 'Column' target when using spikes or synapses,
     *         'Layer1' otherwise.
     *         If the specified target was the '*' wildcard, it returns all GIDs
     *         of the circuit and omits target parsing entirely.
     */
    const brion::GIDSet& getGIDs() const;

    /**
     * @return the GIDs from the 'preTarget' parameter, used for synapse
     *         projections. Empty if parameter not specified.
     */
    const brion::GIDSet& getPreGIDs() const;

    /**
     * Get the specified report name.
     *
     * If the given report is empty, returns 'soma' (for soma and vsd loaders)
     * or 'voltage' (for all others), or the default reports if test data
     * circuit is used.
     *
     * @return the specified report name.
     */
    std::string getReport() const;

    /**
     * Get the specified timestep (dt).
     *
     * @return the specified timestep. If invalid or empty, return 10.
     */
    float getDt() const;

    /** @return URI to spikes source, empty by default */
    std::string getSpikes() const;

    /**
     * Get the specified duration.
     *
     * @return the specified duration. If invalid or empty, return getDt().
     */
    float getDuration() const;

    /**
     * Get the range of values to consider in the input data for rescaling into
     * an output data type that is different than float.
     * The default inputRange depends on VolumeType:
     *   (0.0, 2.0) for SPIKES and SYNAPSES
     *   (-190.0, 0.0) for COMPARTMENTS with TestData, (-80.0, 0.0) otherwise
     *   (-15.0, 0.0) for SOMAS with TestData, [-80.0, 0.0] otherwise
     *   (-100000.0, 300.0) for VSD
     *
     * @return a vector containing the minimum and maximum values that the
     * volume can take before rescaling it
     */
    Vector2f getInputRange() const;

    /**
     * Get the specified path to a dye curve file
     * @return the specified path to the dye curve file
     */
    std::string getDyeCurve() const;

    /**
     * Get the specified resolution in voxels per unit (typically um).
     *
     * @return the specified resolution. If invalid or empty, return 10.
     */
    float getResolution() const;

    /**
     * Get the specified maximum size per block (bytes).
     *
     * @return the specified maximum size per block. If invalid or empty, return
     *         64MB
     */
    size_t getMaxBlockSize() const;

    /**
     * Get the specified cutoff distance in micrometers.
     *
     * @return the specified cutoff distance in micrometers. If invalid or
     *         empty, return 100.
     */
    float getCutoffDistance() const;

    /**
     * Get the additional distance, in micrometers, by which the original data
     * volume will be extended. By default, the volume extension matches the
     * bounding box of the data events.
     *
     * @return the distance to extend the data volume, in micrometers.
     *         If invalid or empty, return 0.
     */
    float getExtendDistance() const;

    /**
     * Get the type of the volume that is being loaded (present in the URI
     * schema)
     * @return the type of the volume
     */
    VolumeType getType() const;

    /**
     * Available functors are "density", "field" and "frequency". If "functor"
     * is unspecified, the default functor for the VolumeType is returned:
     * - FUNCTOR_DENSITY for SYNAPSES
     * - FUNCTOR_FREQUENCY for SPIKES
     * - FUNCTOR_FIELD for COMPARTMENTS, SOMAS and VSD
     *
     * @return the type of the functor to use, use VolumeType default functor
     *          if unspecified.
     */
    FunctorType getFunctorType() const;

    /**
     * @return the path to a reference volume to setup the size and resolution
     *         of the output volme. Empty by default.
     */
    std::string getReferenceVolume() const;

    /** @return the size in voxels along the largest dimension of the volume. */
    size_t getSizeInVoxel() const;

    /** @return description of the volume from the provided URI paramters. */
    std::string getDescription() const;

    /** @return a new image source for the given parameters and pixel type. */
    template< class TImage > ImageSourcePtr< TImage > newImageSource() const;

    /** @return a new functor for the given parameters and pixel type. */
    template< class TImage > EventFunctorPtr< TImage > newFunctor() const;

    /** @return a new event source for the given parameters. */
    EventSourcePtr newEventSource() const;

private:
    class Impl;
    std::unique_ptr< Impl > _impl;
};

std::ostream& operator << ( std::ostream&, const URIHandler& );

}

#endif
