'''
This file is used by the Waf build system to drive this project's build.
'''

from waflib import Context

def options(ctx: Context):
    ctx.load('modules', tooldir='tools')

def configure(ctx: Context):
    ctx.load('modules', tooldir='tools')
    ctx.module('lib/')
    ctx.create_variant_configurations()

def build(ctx: Context):
    ctx.module('lib/')

