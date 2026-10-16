import { defineCollection } from "astro:content";
import { docsCollection } from "@cloudflare/nimbus-docs/content";

export const collections = {
  docs: defineCollection(docsCollection()),
  zh: defineCollection(docsCollection({ base: "zh" })),
};
