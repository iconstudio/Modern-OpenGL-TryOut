module;
#include <Windows.h>
#include <gl\gl.h>

module Glib.Legacy.Lighting;
import <stdexcept>;
import <type_traits>;
import <array>;
import <unordered_set>;

constexpr std::uint32_t	maxLights = GL_MAX_LIGHTS;
constexpr std::uint32_t firstLightIndex = GL_LIGHT0;
constexpr std::uint32_t lastLightIndex = firstLightIndex + maxLights;

static std::array<gl::legacy::Light, maxLights> everyLights{};
static std::unordered_set<std::uint32_t> enabledLights{};
static std::uint32_t nextLightIndex = GL_LIGHT0;

void EnableLight() noexcept;
void DisableLight() noexcept;
void EnableLight(bool flag) noexcept;
void EnableNormal() noexcept;
void DisableNormal() noexcept;
void EnableNormal(bool flag) noexcept;

[[nodiscard]] gl::legacy::Light& AcquireLight(std::uint32_t index);
bool TryAcquireLight(volatile std::uint32_t index, gl::legacy::Light& light) noexcept;
void CastLight(volatile std::uint32_t index, const gl::legacy::Light& light) noexcept;

gl::legacy::Caster::Caster()
	: myIndex(nextLightIndex)
{
	if (enabledLights.size() == maxLights)
	{
		throw std::runtime_error("Too many lights enabled");
	}

	if (enabledLights.empty())
	{
		EnableLight();
		EnableNormal();
	}

	enabledLights.insert(nextLightIndex++);
	this->Enable();
}

gl::legacy::Caster::~Caster()
noexcept
{
	this->Disable();

	enabledLights.erase(myIndex);
	if (enabledLights.empty())
	{
		DisableNormal();
		DisableLight();
	}
}

void
gl::legacy::Caster::SetShadeModel(gl::legacy::ShadeModel shade)
noexcept
{
	::glLighti(myIndex, GL_SHADE_MODEL, static_cast<GLint>(shade));
}

void
gl::legacy::Caster::SetOption(gl::legacy::LightOption option, bool value)
noexcept
{
	::glLighti(myIndex, static_cast<GLenum>(option), static_cast <GLint>(value));
}

void
gl::legacy::Caster::SetAmbient(const gl::legacy::Colour& colour)
noexcept
{
	colour.Extract(cachedAmbient);
	::glLightfv(myIndex, GL_AMBIENT, cachedAmbient);
}

void
gl::legacy::Caster::SetAmbient(gl::legacy::Colour&& colour)
noexcept
{
	colour.Extract(cachedAmbient);
	::glLightfv(myIndex, GL_AMBIENT, cachedAmbient);
}

void
gl::legacy::Caster::SetMaterial(const gl::legacy::Material& material)
noexcept
{
	material.ambient.Extract(cachedAmbient);
	material.diffuse.Extract(cachedDiffuse);
	material.specular.Extract(cachedSpecular);
	material.emission.Extract(cachedEmission);

	::glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, cachedAmbient);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cachedDiffuse);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, cachedSpecular);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, cachedEmission);
	::glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, material.shininess);
}

void
gl::legacy::Caster::SetMaterial(gl::legacy::Material&& material)
noexcept
{
	material.ambient.Extract(cachedAmbient);
	material.diffuse.Extract(cachedDiffuse);
	material.specular.Extract(cachedSpecular);
	material.emission.Extract(cachedEmission);

	::glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, cachedAmbient);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cachedDiffuse);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, cachedSpecular);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, cachedEmission);
	::glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.shininess);
}

void
gl::legacy::Caster::Assign(const gl::legacy::Light& light)
noexcept
{
	GetLight() = light;

	CastLight(myIndex, GetLight());
}

void
gl::legacy::Caster::Assign(gl::legacy::Light&& light)
noexcept
{
	GetLight() = std::move(light);

	CastLight(myIndex, GetLight());
}

void
gl::legacy::Caster::Enable()
noexcept
{
	::glEnable(myIndex);
}

void
gl::legacy::Caster::Disable()
noexcept
{
	::glDisable(myIndex);
}

gl::legacy::Light&
gl::legacy::Caster::GetLight()
volatile noexcept
{
	return AcquireLight(myIndex - firstLightIndex);
}

const gl::legacy::Light&
gl::legacy::Caster::GetLight()
const volatile noexcept
{
	return AcquireLight(myIndex - firstLightIndex);
}

void
EnableLight()
noexcept
{
	EnableLight(true);
}

void
DisableLight()
noexcept
{
	EnableLight(false);
}

void
EnableLight(bool flag)
noexcept
{
	if (flag)
	{
		glEnable(GL_LIGHTING);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}
}

void
EnableNormal()
noexcept
{
	EnableNormal(true);
}

void
DisableNormal()
noexcept
{
	EnableNormal(false);
}

void
EnableNormal(bool flag)
noexcept
{
	if (flag)
	{
		glEnable(GL_NORMALIZE);
	}
	else
	{
		glDisable(GL_NORMALIZE);
	}
}

gl::legacy::Light&
AcquireLight(volatile std::uint32_t index)
{
	if (index < firstLightIndex || lastLightIndex <= index)
	{
		throw std::runtime_error("Invalid light index");
	}

	return everyLights[index - firstLightIndex];
}

bool
TryAcquireLight(volatile std::uint32_t index, gl::legacy::Light& light)
noexcept
{
	if (index < firstLightIndex || lastLightIndex <= index)
	{
		return false;
	}

	light = everyLights[index - firstLightIndex];
	return true;
}

void
CastLight(std::uint32_t index, const gl::legacy::Light& light)
noexcept
{
	float ambient[4]{};
	float diffuse[4]{};
	float specular[4]{};

	light.ambient.Extract(ambient);
	light.diffuse.Extract(diffuse);
	light.specular.Extract(specular);

	::glLightfv(index, GL_AMBIENT, ambient);
	::glLightfv(index, GL_DIFFUSE, diffuse);
	::glLightfv(index, GL_SPECULAR, specular);

	::glLightfv(index, GL_POSITION, light.position);

	::glLightf(index, GL_SPOT_EXPONENT, light.spotExponent);
	::glLightf(index, GL_SPOT_CUTOFF, light.spotCutoff);
	::glLightfv(index, GL_SPOT_DIRECTION, light.spotDirection);

	::glLightf(index, GL_CONSTANT_ATTENUATION, light.constantAttenuation);
	::glLightf(index, GL_LINEAR_ATTENUATION, light.linearAttenuation);
	::glLightf(index, GL_QUADRATIC_ATTENUATION, light.quadraticAttenuation);
}
