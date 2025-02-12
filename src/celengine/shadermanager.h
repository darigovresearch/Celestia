// shadermanager.h
//
// Copyright (C) 2001-2004, Chris Laurel <claurel@shatters.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <cstdint>
#include <map>
#include <string>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <celutil/color.h>
#include <celengine/glshader.h>


class Atmosphere;
class LightingState;

class ShaderProperties
{
 public:
    ShaderProperties() = default;
    bool usesShadows() const;
    bool usesFragmentLighting() const;
    bool usesTangentSpaceLighting() const;
    bool usePointSize() const;

    unsigned int getEclipseShadowCountForLight(unsigned int lightIndex) const;
    void setEclipseShadowCountForLight(unsigned int lightIndex, unsigned int shadowCount);
    bool hasEclipseShadows() const;
    bool hasRingShadowForLight(unsigned int lightIndex) const;
    void setRingShadowForLight(unsigned int lightIndex, bool enabled);
    bool hasRingShadows() const;
    void setSelfShadowForLight(unsigned int lightIndex, bool enabled);
    bool hasSelfShadowForLight(unsigned int lightIndex) const;
    bool hasSelfShadows() const;
    void setCloudShadowForLight(unsigned int lightIndex, bool enabled);
    bool hasCloudShadowForLight(unsigned int lightIndex) const;
    bool hasCloudShadows() const;
    bool hasShadowMap() const;

    bool hasShadowsForLight(unsigned int) const;
    bool hasSharedTextureCoords() const;
    bool hasSpecular() const;
    bool hasScattering() const;
    bool isViewDependent() const;

 enum
 {
     DiffuseTexture          =    0x01,
     SpecularTexture         =    0x02,
     NormalTexture           =    0x04,
     NightTexture            =    0x08,
     SpecularInDiffuseAlpha  =    0x10,
     RingShadowTexture       =    0x20,
     OverlayTexture          =    0x40,
     CloudShadowTexture      =    0x80,
     CompressedNormalTexture =   0x100,
     EmissiveTexture         =   0x200,
     ShadowMapTexture        =   0x400,
     VertexOpacities         =   0x800,
     VertexColors            =  0x1000,
     Scattering              =  0x2000,
     PointSprite             =  0x4000,
     SharedTextureCoords     =  0x8000,
     StaticPointSize         = 0x10000,
     LineAsTriangles         = 0x20000,
 };

 enum
 {
     DiffuseModel          = 0x0001,
     SpecularModel         = 0x0002,
     RingIllumModel        = 0x0004,
     PerPixelSpecularModel = 0x0008,
     OrenNayarModel        = 0x0010,
     AtmosphereModel       = 0x0020,
     LunarLambertModel     = 0x0040,
     ParticleDiffuseModel  = 0x0080,
     EmissiveModel         = 0x0100,
     ParticleModel         = 0x0200,
     UnlitModel            = 0x0400,
 };

 enum
 {
     VolumetricScatteringEffect      = 0x0001,
     VolumetricAbsorptionEffect      = 0x0002,
     VolumetricEmissionEffect        = 0x0004,
 };

 enum
 {
     FisheyeOverrideModeNone     = 0,
     FisheyeOverrideModeEnabled  = 1,
     FisheyeOverrideModeDisabled = 2,
 };

 public:
    unsigned short nLights{ 0 };
    unsigned short lightModel{ DiffuseModel };
    unsigned long texUsage{ 0 };

    // Effects that may be applied with any light model
    unsigned short effects{ 0 };

    // Eight bits per light, up to four lights
    // For each light:
    //   Bits 0-1, eclipse shadow count, from 0-3
    //   Bit  2,   on if there are ring shadows
    //   Bit  3,   on for self shadowing
    //   Bit  4,   on for cloud shadows
    std::uint32_t shadowCounts{ 0 };

    int fishEyeOverride { FisheyeOverrideModeNone };

 private:
    enum
    {
        ShadowBitsPerLight = 8,
    };

    enum
    {
        EclipseShadowMask = 0x3,
        RingShadowMask    = 0x4,
        SelfShadowMask    = 0x8,
        CloudShadowMask   = 0x10,
        AnyEclipseShadowMask = 0x03030303,
        AnyRingShadowMask    = 0x04040404,
        AnySelfShadowMask    = 0x08080808,
        AnyCloudShadowMask   = 0x10101010,
    };

    friend class ShaderManager;
};


static const unsigned int MaxShaderLights = 4;
static const unsigned int MaxShaderEclipseShadows = 3;
struct CelestiaGLProgramLight
{
    Vec3ShaderParameter direction;
    Vec3ShaderParameter diffuse;
    Vec3ShaderParameter specular;
    Vec3ShaderParameter halfVector;
    FloatShaderParameter brightness; // max of diffuse r, g, b
};

struct CelestiaGLProgramShadow
{
    Vec4ShaderParameter texGenS;
    Vec4ShaderParameter texGenT;
    FloatShaderParameter falloff;
    FloatShaderParameter maxDepth;
};

class CelestiaGLProgram
{
 public:
    CelestiaGLProgram(GLProgram& _program);
    CelestiaGLProgram(GLProgram& _program, const ShaderProperties&);
    ~CelestiaGLProgram();

    void use() const { program->use(); }

    void setLightParameters(const LightingState& ls,
                            Color materialDiffuse,
                            Color materialSpecular,
                            Color materialEmissive);
    void setEclipseShadowParameters(const LightingState& ls,
                                    const Eigen::Vector3f& scale,
                                    const Eigen::Quaternionf& orientation);
    void setAtmosphereParameters(const Atmosphere& atmosphere,
                                 float atmPlanetRadius,
                                 float objRadius);
    void setMVPMatrices(const Eigen::Matrix4f& p, const Eigen::Matrix4f& m = Eigen::Matrix4f::Identity());

    enum
    {
        VertexCoordAttributeIndex   = 0,
        NormalAttributeIndex        = 1,
        TextureCoord0AttributeIndex = 2,
        TextureCoord1AttributeIndex = 3,
        TextureCoord2AttributeIndex = 4,
        TextureCoord3AttributeIndex = 5,
        TangentAttributeIndex       = 6,
        PointSizeAttributeIndex     = 7,
        ColorAttributeIndex         = 8,
        IntensityAttributeIndex     = 9,
        NextVCoordAttributeIndex    = 10,
        ScaleFactorAttributeIndex   = 11,
    };

 public:
    CelestiaGLProgramLight lights[MaxShaderLights];
    Vec3ShaderParameter fragLightColor[MaxShaderLights];
    Vec3ShaderParameter fragLightSpecColor[MaxShaderLights];
    FloatShaderParameter fragLightBrightness[MaxShaderLights];
    FloatShaderParameter ringShadowLOD[MaxShaderLights];
    Vec3ShaderParameter eyePosition;
    FloatShaderParameter shininess;
    Vec3ShaderParameter ambientColor;
    FloatShaderParameter opacity;

    FloatShaderParameter ringWidth;
    FloatShaderParameter ringRadius;
    Vec4ShaderParameter ringPlane;
    Vec3ShaderParameter ringCenter;

    // Mix of Lambertian and "lunar" (Lommel-Seeliger) photometric models.
    // 0 = pure Lambertian, 1 = L-S
    FloatShaderParameter lunarLambert;

    // Diffuse texture coordinate offset
    FloatShaderParameter textureOffset;

    // Cloud shadow parameters
    // Height of cloud layer above planet, in units of object radius
    FloatShaderParameter cloudHeight;
    FloatShaderParameter shadowTextureOffset;

    // Parameters for atmospheric scattering; all distances are normalized for
    // a unit sphere.
    FloatShaderParameter mieCoeff;
    FloatShaderParameter mieScaleHeight;
    // Value of k for Schlick approximation to Henyey-Greenstein phase function
    // A value of 0 is isotropic, negative values a primarily backscattering,
    // positive values are forward scattering.
    FloatShaderParameter miePhaseAsymmetry;

    // Rayleigh scattering terms. There are three scattering coefficients: red,
    // green, and blue light. To simulate Rayleigh scattering, the coefficients
    // should be in ratios that fit 1/wavelength^4, but other values may be used
    // to simulate different types of wavelength dependent scattering.
    Vec3ShaderParameter rayleighCoeff;
    FloatShaderParameter rayleighScaleHeight;

    // Precomputed sum and inverse sum of Rayleigh and Mie scattering coefficients
    Vec3ShaderParameter scatterCoeffSum;
    Vec3ShaderParameter invScatterCoeffSum;
    // Precomputed sum of absorption and scattering coefficients--identical to
    // scatterCoeffSum when there is no absorption.
    Vec3ShaderParameter extinctionCoeff;

    // Radius of sphere for atmosphere--should be significantly larger than
    // scale height. Three components:
    //    x = radius
    //    y = radius^2
    //    z = 1/radius
    Vec3ShaderParameter atmosphereRadius;

    // Scale factor for point sprites
    FloatShaderParameter pointScale;

    // Used to draw line as triangles
    FloatShaderParameter lineWidthX;
    FloatShaderParameter lineWidthY;

    // Color sent as a uniform
    Vec4ShaderParameter color;

    // Matrix used to project to light space
    Mat4ShaderParameter ShadowMatrix0;

    CelestiaGLProgramShadow shadows[MaxShaderLights][MaxShaderEclipseShadows];

    FloatShaderParameter floatParam(const std::string&);
    IntegerShaderParameter intParam(const std::string&);
    IntegerShaderParameter samplerParam(const std::string&);
    Vec3ShaderParameter vec3Param(const std::string&);
    Vec4ShaderParameter vec4Param(const std::string&);
    Mat3ShaderParameter mat3Param(const std::string&);
    Mat4ShaderParameter mat4Param(const std::string&);

    Mat4ShaderParameter ModelViewMatrix;
    Mat4ShaderParameter ProjectionMatrix;
    Mat4ShaderParameter MVPMatrix;

    int attribIndex(const std::string&) const;

 private:
    void initCommonParameters();
    void initParameters();
    void initSamplers();

    GLProgram* program;
    const ShaderProperties props;
};


class ShaderManager
{
 public:
    ShaderManager();
    ~ShaderManager();

    CelestiaGLProgram* getShader(const ShaderProperties&);
    CelestiaGLProgram* getShader(const std::string&);
    CelestiaGLProgram* getShader(const std::string&, const std::string&, const std::string&);

    void setFisheyeEnabled(bool enabled);

 private:
    CelestiaGLProgram* buildProgram(const ShaderProperties&);
    CelestiaGLProgram* buildProgram(const std::string&, const std::string&);

    GLVertexShader* buildVertexShader(const ShaderProperties&);
    GLFragmentShader* buildFragmentShader(const ShaderProperties&);

    GLVertexShader* buildRingsVertexShader(const ShaderProperties&);
    GLFragmentShader* buildRingsFragmentShader(const ShaderProperties&);

    GLVertexShader* buildAtmosphereVertexShader(const ShaderProperties&);
    GLFragmentShader* buildAtmosphereFragmentShader(const ShaderProperties&);

    GLVertexShader* buildEmissiveVertexShader(const ShaderProperties&);
    GLFragmentShader* buildEmissiveFragmentShader(const ShaderProperties&);

    GLVertexShader* buildParticleVertexShader(const ShaderProperties&);
    GLFragmentShader* buildParticleFragmentShader(const ShaderProperties&);

    std::map<ShaderProperties, CelestiaGLProgram*> dynamicShaders;
    std::map<std::string, CelestiaGLProgram*> staticShaders;

    bool fisheyeEnabled { false };
};
