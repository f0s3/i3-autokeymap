const assert = require('chai').assert
const expect = require('chai').expect
const i3autokeymap = require('../i3-autokeymap')

let workspaceActivityArraySample = [
	{ num: 1, focused: false },
	{ num: 2, focused: true },
	{ num: 3, focused: false },
	{ num: 4, focused: false },
	{ num: 5, focused: false }
]

describe('i3-autokeymap', () => {
	it('focusedWorkspace should be a function', () => {
		assert.isFunction(i3autokeymap.focusedWorkspace)
	})
	
	it('workspaceActivityArray should be an array', () => {
		assert.isArray(workspaceActivityArraySample)
	})
	
	it('workspaceActivityArray should have objects inside', () => {
		workspaceActivityArraySample.forEach(i => assert.isObject(i))
	})
	
	it('workspaceActivityArray should have second workspace as focused', () => {
		expect(workspaceActivityArraySample[1]).to.eql({ num: 2, focused: true })
	})
})
