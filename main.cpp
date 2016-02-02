#include "context.h"
#include "renderer.h"
#include "glm/glm.hpp"

int main(int argc, char** argv)
{
	Renderer renderer;
	Context context(renderer);

    printf("%d, %d\n", sizeof(glm::vec3), sizeof(float));
    context.init(1024, 768, "voitures", 0);
    while (context.eventLoop())
    {
        context.clean();
        context.show();
    }

    return 0;
}
