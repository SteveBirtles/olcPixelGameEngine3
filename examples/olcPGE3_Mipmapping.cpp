/*
	olc::PixelGameEngine3 Example - Mipmapping

	Press [Space] to toggle mipmapping on / off

	Licenced under the OLC-3 License
*/


#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

class Example_Mipmapping : public olc::PixelGameEngine {
   protected:
    static constexpr int32_t nTextureSize = 128;
    static constexpr int32_t nGridSize = 64;

    olc::Image imgRegular;
    olc::Image imgMipmapped;

    std::vector<olc::vf4d> vVertices;
    std::vector<olc::vf2d> vTextureCoordinates;
    std::vector<olc::Pixel> vColours;

    olc::mf4d matProjection;
    olc::vf4d vCamera = {0.0f, -1.0f, 0.0f};

    bool bMipmapped = false;

   public:
    bool OnUserCreate() override {
        olc::ImageConfig cfgRegular;
        cfgRegular.Filtered = true;
        cfgRegular.Clamp = false;
        CreateImage(imgRegular, {nTextureSize, nTextureSize}, cfgRegular);

        olc::ImageConfig cfgMipmapped;
        cfgMipmapped.Filtered = true;
        cfgMipmapped.Clamp = false;
        cfgMipmapped.Mipmapped = true;
        CreateImage(imgMipmapped, {nTextureSize, nTextureSize}, cfgMipmapped);

        const olc::Pixel stripes[] = {olc::Pixel(255, 0, 0),   olc::Pixel(255, 128, 0), olc::Pixel(255, 255, 0), olc::Pixel(0, 255, 0),
                                      olc::Pixel(0, 255, 255), olc::Pixel(0, 0, 255),   olc::Pixel(128, 0, 255), olc::Pixel(255, 0, 255)};
        constexpr int32_t nStripeWidth = 4;
        constexpr int32_t nStripeCount = int32_t(std::size(stripes));

        for (int32_t y = 0; y < nTextureSize; y++)
            for (int32_t x = 0; x < nTextureSize; x++) {
                const olc::Pixel p = stripes[((x + y) / nStripeWidth) % nStripeCount];
                imgRegular.Pixel({x, y}) = p;
                imgMipmapped.Pixel({x, y}) = p;
            }

        for (int32_t z = 0; z < nGridSize; z++)
            for (int32_t x = 0; x < nGridSize; x++) {
                const float x0 = float(x - nGridSize / 2);
                const float x1 = x0 + 1.0f;
                const float z0 = float(z) + 0.5f;
                const float z1 = z0 + 1.0f;

                vVertices.push_back({x0, 0.0f, z0});
                vTextureCoordinates.push_back({0.0f, 1.0f});
                vVertices.push_back({x0, 0.0f, z1});
                vTextureCoordinates.push_back({0.0f, 0.0f});
                vVertices.push_back({x1, 0.0f, z1});
                vTextureCoordinates.push_back({1.0f, 0.0f});
                vVertices.push_back({x0, 0.0f, z0});
                vTextureCoordinates.push_back({0.0f, 1.0f});
                vVertices.push_back({x1, 0.0f, z1});
                vTextureCoordinates.push_back({1.0f, 0.0f});
                vVertices.push_back({x1, 0.0f, z0});
                vTextureCoordinates.push_back({1.0f, 1.0f});
            }
        vColours.assign(vVertices.size(), olc::Colour::WHITE);

        matProjection.perspective(70.0f * 3.14159f / 180.0f, float(ScreenSize().x) / float(ScreenSize().y), 0.1f, 200.0f);

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        if (keyboard.GetKey(olc::Key::SPACE).bPressed) bMipmapped = !bMipmapped;

        const float fSpeed = 5.0f * fElapsedTime;
        if (keyboard.GetKey(olc::Key::W).bHeld) vCamera.z -= fSpeed;
        if (keyboard.GetKey(olc::Key::S).bHeld) vCamera.z += fSpeed;
        if (keyboard.GetKey(olc::Key::A).bHeld) vCamera.x += fSpeed;
        if (keyboard.GetKey(olc::Key::D).bHeld) vCamera.x -= fSpeed;
        
        draw.Clear(olc::Pixel(100, 150, 220));

        draw.SetProjectionMatrix(matProjection);

        olc::mf4d matViewRotateX, matViewTranslate;
        matViewRotateX.rotateX(3.14159f);
        matViewTranslate.translate(vCamera);
        draw.SetViewMatrix(matViewRotateX * matViewTranslate);

        olc::mf4d matWorld;
        draw.SetModelMatrix(matWorld);

        draw.SetCullMode(olc::CullMode::None);
        draw.Mesh(olc::Structure::List, vVertices, vColours, vTextureCoordinates, bMipmapped ? imgMipmapped : imgRegular);

        draw.StringProp({4, 4}, bMipmapped ? "Mipmapping: ON (Space to toggle, WASD to move)" : "Mipmapping: OFF (Space to toggle, WASD to move)", bMipmapped ? olc::Colour::GREEN : olc::Colour::RED, {2.0f, 2.0f});

        return true;
    }
};

int main() {
    Example_Mipmapping demo;

    olc::PGEConfig config;
    config.vScreenSize = {640, 400};
    config.vPixelSize = {2, 2};
    config.bAntiAliasMainScreen = false;
    config.sAppName = "Example - Mipmapping";

    if (demo.Construct(config)) demo.Start();

    return 0;
}
