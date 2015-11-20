/* Copyright (c) 2014-2015, EPFL/Blue Brain Project
 *                          Stefan.Eilemann@epfl.ch
 *                          Jafet.VillafrancaDiaz@epfl.ch
 */

#ifndef FIVOX_COMPARTMENTLOADER_H
#define FIVOX_COMPARTMENTLOADER_H

#include <fivox/eventSource.h> // base class

namespace fivox
{
/** Loads compartment report data to be sampled by an EventFunctor. */
class CompartmentLoader : public EventSource
{
public:
    /**
    * Construct a new compartment event source.
    *
    * @param params the URIHandler object containing the parameters
    * to define the event source
    * @throw H5::exception or std::exception on error
    */
    explicit CompartmentLoader( const URIHandler& params );
    virtual ~CompartmentLoader(); //!< Destruct this compartment event source

private:
    /** @name Abstract interface implementation */
    //@{
    Vector2f _getTimeRange() const final;
    bool _load( float time ) final;
    SourceType _getType() const final { return SOURCE_FRAME; }
    //@}

    class Impl;
    std::unique_ptr< Impl > _impl;
};
}

#endif
