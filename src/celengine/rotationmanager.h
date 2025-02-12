// rotationmanager.h
//
// Copyright (C) 2006 Chris Laurel <claurel@shatters.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#ifndef CELENGINE_ROTATIONMANAGER_H_
#define CELENGINE_ROTATIONMANAGER_H_

#include <celephem/rotation.h>
#include <celutil/resmanager.h>
#include <string>
#include <map>


class RotationModelInfo : public ResourceInfo<celestia::ephem::RotationModel>
{
 public:
    std::string source;
    fs::path path;

    RotationModelInfo(const std::string& _source,
                      const fs::path& _path = "") :
        source(_source), path(_path) {};

    fs::path resolve(const fs::path&) override;
    celestia::ephem::RotationModel* load(const fs::path&) override;
};

inline bool operator<(const RotationModelInfo& ti0,
                      const RotationModelInfo& ti1)
{
    if (ti0.source == ti1.source)
        return ti0.path < ti1.path;
    else
        return ti0.source < ti1.source;
}

typedef ResourceManager<RotationModelInfo> RotationModelManager;

extern RotationModelManager* GetRotationModelManager();

#endif // CELENGINE_ROTATIONMANAGER_H_

