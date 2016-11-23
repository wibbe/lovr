#include "graphics/model.h"
#include "graphics/graphics.h"
#include <stdlib.h>

static void visitNode(ModelData* modelData, ModelNode* node, mat4 transform, vec_float_t* vertices, vec_uint_t* indices) {
  mat4 newTransform;

  if (!transform) {
    newTransform = mat4_init();
  } else {
    newTransform = mat4_copy(transform);
  }

  mat4_multiply(newTransform, node->transform);

  int indexOffset = vertices->length / 3;

  // Meshes
  for (int m = 0; m < node->meshes.length; m++) {
    ModelMesh* mesh = modelData->meshes.data[node->meshes.data[m]];

    // Transformed vertices
    for (int v = 0; v < mesh->vertices.length; v++) {
      ModelVertex vertex = mesh->vertices.data[v];

      float transformedVertex[4] = {
        vertex.x,
        vertex.y,
        vertex.z,
        1.f
      };

      mat4_multiplyVector(newTransform, transformedVertex);
      vec_pusharr(vertices, transformedVertex, 3);

      if (modelData->hasNormals) {
        ModelVertex normal = mesh->normals.data[v];
        vec_push(vertices, normal.x);
        vec_push(vertices, normal.y);
        vec_push(vertices, normal.z);
      }
    }

    // Face vertex indices
    for (int f = 0; f < mesh->faces.length; f++) {
      ModelFace face = mesh->faces.data[f];

      for (int v = 0; v < face.indices.length; v++) {
        vec_push(indices, face.indices.data[v] + indexOffset);
      }
    }
  }

  for (int c = 0; c < node->children.length; c++) {
    visitNode(modelData, node->children.data[c], newTransform, vertices, indices);
  }

  mat4_deinit(newTransform);
}

Model* lovrModelCreate(void* data, int size) {
  Model* model = lovrAlloc(sizeof(Model), lovrModelDestroy);
  if (!model) return NULL;

  model->modelData = lovrModelDataCreate(data, size);
  if (!model->modelData) return NULL;

  vec_float_t vertices;
  vec_init(&vertices);

  vec_uint_t indices;
  vec_init(&indices);

  visitNode(model->modelData, model->modelData->root, NULL, &vertices, &indices);

  BufferFormat format;
  vec_init(&format);

  BufferAttribute position = { .name = "lovrPosition", .type = BUFFER_FLOAT, .count = 3 };
  vec_push(&format, position);

  if (model->modelData->hasNormals) {
    BufferAttribute normal = { .name = "lovrNormal", .type = BUFFER_FLOAT, .count = 3 };
    vec_push(&format, normal);
  }

  int components = model->modelData->hasNormals ? 6 : 3;

  model->buffer = lovrBufferCreate(vertices.length / components, &format, BUFFER_TRIANGLES, BUFFER_STATIC);
  lovrBufferSetVertices(model->buffer, (void*) vertices.data, vertices.length * sizeof(float));
  lovrBufferSetVertexMap(model->buffer, indices.data, indices.length);

  model->texture = NULL;

  vec_deinit(&format);
  vec_deinit(&vertices);
  vec_deinit(&indices);
  return model;
}

void lovrModelDestroy(const Ref* ref) {
  Model* model = containerof(ref, Model);
  lovrRelease(&model->modelData->ref);
  lovrRelease(&model->buffer->ref);
  free(model);
}

void lovrModelDraw(Model* model, float x, float y, float z, float size, float angle, float ax, float ay, float az) {
  lovrGraphicsPush();
  lovrGraphicsTransform(x, y, z, size, size, size, angle, ax, ay, az);
  lovrBufferDraw(model->buffer);
  lovrGraphicsPop();
}

Texture* lovrModelGetTexture(Model* model) {
  return model->texture;
}

void lovrModelSetTexture(Model* model, Texture* texture) {
  model->texture = texture;
}
